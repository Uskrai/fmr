use std::{
    ffi::OsStr,
    fs::DirEntry,
    path::{Path, PathBuf},
    sync::Arc,
};

use serde::{Deserialize, Serialize};

use crate::UnsignedExt;

pub fn get_folder_by(
    path: impl AsRef<Path>,
    direction: isize,
    sorter: impl FnOnce(&mut Vec<DirEntry>),
) -> Option<PathBuf> {
    let parent = path.as_ref().parent()?;

    let dir = std::fs::read_dir(parent).ok()?;

    let mut dir = dir.filter_map(|s| s.ok()).collect::<Vec<_>>();

    sorter(&mut dir);

    let mut index = 0;

    for (i, it) in dir.iter().enumerate() {
        if it.path() == path.as_ref() {
            index = i;
        }
    }

    let index = index.checked_add_signed_ext(direction)?;

    dir.get(index).map(|it| it.path())
}

pub fn get_sorted_folder_by(
    path: impl AsRef<Path>,
    direction: isize,
    sorter: impl Fn(&DirEntry, &DirEntry) -> std::cmp::Ordering,
) -> Option<PathBuf> {
    get_folder_by(path, direction, |dir| dir.sort_by(sorter))
}

pub fn get_natural_sorted_folder_by(path: impl AsRef<Path>, direction: isize) -> Option<PathBuf> {
    get_folder_by(path, direction, |dir| {
        dir.sort_by(
            |a, b| match (a.file_name().to_str(), b.file_name().to_str()) {
                (Some(a), Some(b)) => natord::compare(a, b),
                _ => a.file_name().cmp(&b.file_name()),
            },
        );
    })
}

pub fn compare_natural(a: &OsStr, b: &OsStr) -> std::cmp::Ordering {
    compare_str(a, b, natord::compare)
}

pub fn compare_str(
    a: &OsStr,
    b: &OsStr,
    fun: impl FnOnce(&str, &str) -> std::cmp::Ordering,
) -> std::cmp::Ordering {
    match (a.to_str(), b.to_str()) {
        (Some(a), Some(b)) => fun(a, b),
        _ => a.cmp(b),
    }
}

#[derive(Clone, Debug, Serialize, Deserialize, PartialEq)]
pub struct PathSorterInner {
    pub natural: bool,
    pub last_modified: bool,
    pub ascending: bool,
}

impl Default for PathSorterInner {
    fn default() -> Self {
        Self {
            natural: true,
            last_modified: false,
            ascending: true,
        }
    }
}

impl PathSorterInner {
    pub fn compare_path(&self, a: &Path, b: &Path) -> std::cmp::Ordering {
        let (a, b) = if self.ascending { (a, b) } else { (b, a) };

        if self.last_modified {
            let ord = compare_last_modified(a, b);

            if !ord.is_eq() {
                return ord;
            }
        }

        if self.natural {
            let ord = compare_natural(a.file_name().unwrap(), b.file_name().unwrap());

            if !ord.is_eq() {
                return ord;
            }
        }

        a.cmp(b)
    }
}

#[derive(Clone, Debug, PartialEq)]
pub struct PathSorterSetting {
    inner: PathSorterInner,
    subscriber: PathSorterSender,
}

impl Serialize for PathSorterSetting {
    fn serialize<S>(&self, serializer: S) -> Result<S::Ok, S::Error>
    where
        S: serde::Serializer,
    {
        self.inner.serialize(serializer)
    }
}

impl<'de> Deserialize<'de> for PathSorterSetting {
    fn deserialize<D>(deserializer: D) -> Result<Self, D::Error>
    where
        D: serde::Deserializer<'de>,
    {
        let inner = PathSorterInner::deserialize(deserializer)?;
        let (tx, _) = tokio::sync::watch::channel(inner.clone());

        Ok(Self {
            inner,
            subscriber: PathSorterSender(Arc::new(tx)),
        })
    }
}

#[derive(Clone, Debug)]
pub struct PathSorterSender(Arc<tokio::sync::watch::Sender<PathSorterInner>>);

impl PartialEq for PathSorterSender {
    fn eq(&self, other: &Self) -> bool {
        Arc::ptr_eq(&self.0, &other.0)
    }
}

impl Default for PathSorterSender {
    fn default() -> Self {
        let (tx, _) = tokio::sync::watch::channel(PathSorterInner::default());

        Self(Arc::new(tx))
    }
}

impl Default for PathSorterSetting {
    fn default() -> Self {
        Self {
            inner: PathSorterInner {
                natural: true,
                last_modified: false,
                ascending: true,
            },

            subscriber: PathSorterSender::default(),
        }
    }
}

pub fn compare_last_modified(a: &Path, b: &Path) -> std::cmp::Ordering {
    let a = a.metadata().and_then(|it| it.modified());
    let b = b.metadata().and_then(|it| it.modified());

    let (a, b) = match (a, b) {
        (Ok(a), Ok(b)) => (a, b),
        (_, _) => return std::cmp::Ordering::Equal,
    };

    a.cmp(&b)
}

pub struct PathSorterSubscriber(tokio::sync::watch::Receiver<PathSorterInner>);

impl PathSorterSubscriber {
    pub fn compare_path(&self, a: &Path, b: &Path) -> std::cmp::Ordering {
        self.0.borrow().compare_path(a, b)
    }
}

impl PathSorterSetting {
    pub fn subscribe(&self) -> PathSorterSubscriber {
        let it = self.subscriber.0.subscribe();
        self.sync();
        PathSorterSubscriber(it)
    }

    pub fn sync(&self) {
        if *self.subscriber.0.borrow() != self.inner {
            self.subscriber.0.send(self.inner.clone()).ok();
        }
    }

    pub fn compare_path(&self, a: &Path, b: &Path) -> std::cmp::Ordering {
        self.inner.compare_path(a, b)
    }

    pub fn natural_mut(&mut self) -> &mut bool {
        &mut self.inner.natural
    }

    pub fn last_modified_mut(&mut self) -> &mut bool {
        &mut self.inner.last_modified
    }

    pub fn ascending_mut(&mut self) -> &mut bool {
        &mut self.inner.ascending
    }
}

pub mod path_serde {
    use std::{ffi::OsString, path::PathBuf};

    use serde::{Deserialize, Deserializer, Serialize, Serializer};

    #[derive(Deserialize, Serialize)]
    pub enum StringOrOsStr<'a> {
        String(&'a str),
        OsString(OsString),
    }

    pub fn serialize<S: Serializer>(v: &std::path::Path, s: S) -> Result<S::Ok, S::Error> {
        let os_str = v.as_os_str();
        if let Some(v) = v.to_str() {
            StringOrOsStr::String(v).serialize(s)
        } else {
            StringOrOsStr::OsString(os_str.to_os_string()).serialize(s)
        }
    }

    pub fn deserialize<'de, D: Deserializer<'de>>(d: D) -> Result<PathBuf, D::Error> {
        match StringOrOsStr::deserialize(d)? {
            StringOrOsStr::String(s) => Ok(PathBuf::from(s)),
            StringOrOsStr::OsString(s) => Ok(PathBuf::from(s)),
        }
    }
}
