use std::{
    ffi::OsStr,
    fs::DirEntry,
    path::{Path, PathBuf},
};

use crate::UnsignedExt;

pub fn get_folder_by(
    path: impl AsRef<Path>,
    direction: isize,
    sorter: impl FnOnce(&mut Vec<DirEntry>),
) -> Option<PathBuf> {
    let canonicalized = path.as_ref().canonicalize().ok()?;
    let parent = canonicalized.parent()?;

    let dir = std::fs::read_dir(parent).ok()?;

    let mut dir = dir.filter_map(|s| s.ok()).collect::<Vec<_>>();

    sorter(&mut dir);

    let mut index = 0;

    for (i, it) in dir.iter().enumerate() {
        if it.path() == canonicalized {
            index = i;
        }
    }

    let index = index.checked_add_signed_ext(direction.into())?;

    dir.get(index).map(|it| it.path())
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
    match (a.to_str(), b.to_str()) {
        (Some(a), Some(b)) => natord::compare(a, b),
        _ => a.cmp(b),
    }
}
