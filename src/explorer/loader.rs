use std::{
    collections::{HashMap, HashSet},
    ops::Range,
    path::PathBuf,
    sync::{
        atomic::{AtomicUsize, Ordering},
        Arc, Weak,
    },
    time::Instant,
};

use derivative::Derivative;
use futures::StreamExt;
use itertools::Itertools;
use parking_lot::{Mutex, RwLock};
use serde::{Deserialize, Serialize};
use sha2::Digest;
use tokio::{
    sync::{oneshot, watch, OwnedSemaphorePermit, Semaphore},
    task::yield_now,
};
use tracing::instrument;
use zip::ZipArchive;

use super::{Explorer, PathExplorerItem};

#[derive(Clone, Copy, PartialEq, Eq, Debug, Derivative, Serialize, Deserialize)]
#[derivative(Default)]
pub enum FilterType {
    /// Nearest Neighbor
    #[derivative(Default)]
    Nearest,

    /// Linear Filter
    Triangle,

    /// Cubic Filter
    CatmullRom,

    /// Gaussian Filter
    Gaussian,

    /// Lanczos with window 3
    Lanczos3,
}

impl FilterType {
    pub fn to_image(self) -> image::imageops::FilterType {
        use image::imageops::FilterType as F;
        match self {
            FilterType::Nearest => F::Nearest,
            FilterType::Triangle => F::Triangle,
            FilterType::CatmullRom => F::CatmullRom,
            FilterType::Gaussian => F::Gaussian,
            FilterType::Lanczos3 => F::Lanczos3,
        }
    }
}

#[derive(Clone, Derivative)]
#[derivative(Debug)]
pub struct ExplorerLoader {
    #[derivative(Debug = "ignore")]
    pub explorer: Arc<RwLock<Explorer<PathExplorerItem>>>,
    pub path: PathBuf,
    pub selected_entry: Option<PathBuf>,
    pub setting_receiver: watch::Receiver<ExplorerLoaderSetting>,
    pub cache: ExplorerLoaderCache,
    #[derivative(Debug = "ignore")]
    pub ctx: egui::Context,
}

#[derive(Clone, PartialEq, Eq, Debug, Default, Serialize, Deserialize)]
#[serde(default)]
pub struct ExplorerLoaderSetting {
    #[serde(skip)]
    pub index: usize,
    pub entry_setting: ExplorerEntryLoaderSetting,
}

impl ExplorerLoaderSetting {
    pub fn load_priorities(&self, size: usize) -> impl Iterator<Item = usize> {
        crate::split_and_interleave_at(0..size, self.index)
    }
}

impl ExplorerLoader {
    #[instrument(skip(self))]
    pub async fn load(self) {
        let Self {
            explorer,
            path,
            selected_entry,
            mut setting_receiver,
            cache,
            ctx,
        } = self.clone();

        let dir = match std::fs::read_dir(&path) {
            Ok(dir) => dir,
            _ => return,
        };

        let mut content = vec![];

        for it in dir.flatten() {
            content.push(it);
        }

        content.sort_by(|a, b| crate::path::compare_natural(&a.file_name(), &b.file_name()));
        explorer.write().content.reserve(content.len());
        let priority = Arc::new(Mutex::new(PriorityRange::new(0..content.len())));
        let semaphore = PrioritySemaphore::new(4, 0, priority);

        let mut child = Vec::new();
        let size = 300;

        let default_texture = image::DynamicImage::new_rgb8(1, 1);
        let default_texture = crate::image::ImageData::StaticImage(default_texture)
            .into_allocatable((size as u32, size as u32))
            .into_egui()
            .alloc(ctx.tex_manager(), format!("{:?} default", path));

        let entry_setting = Arc::new(Mutex::new(
            self.setting_receiver.borrow().entry_setting.clone(),
        ));

        for (i, entry) in content.into_iter().enumerate() {
            if let Some(it) = PathExplorerItem::new(entry.path()) {
                explorer.write().content.push(it);

                let explorer_ = explorer.clone();
                let path = entry.path();
                let ctx = ctx.clone();
                let default_texture = default_texture.clone();
                let semaphore = semaphore.clone();
                let entry_setting = entry_setting.clone();
                let cache = cache.clone();

                child.push(crate::spawn_and_abort_on_drop(async move {
                    let explorer = explorer_;
                    let waiter = wait_fn(move || {
                        // let semaphore = semaphore.clone();

                        async {}
                    });

                    tokio::task::yield_now().await;
                    let _permit = semaphore.acquire(i).await.unwrap();
                    let image =
                        ExplorerEntryLoader::new(path.clone(), waiter, entry_setting, cache)
                            .search()
                            .await;

                    let texture = match image {
                        Some(image) => {
                            image.alloc(ctx.tex_manager(), path.to_string_lossy().to_string())
                        }
                        None => default_texture,
                    };

                    explorer.write().content[i].thumbnail = texture.into_view_state();
                    ctx.request_repaint();
                }));

                if Some(entry.path()) == selected_entry {
                    explorer.write().set_index(i);
                }
            }
        }

        explorer.write().scroll_to_current();

        loop {
            {
                let it = setting_receiver.borrow();
                semaphore
                    .interleave_index
                    .store(it.index, Ordering::Release);
                *entry_setting.lock() = it.entry_setting.clone();
            }

            if setting_receiver.changed().await.is_err() {
                break;
            }
        }
    }
}

#[derive(Clone, Debug)]
pub struct PriorityRange {
    range: Range<usize>,
    filtered: HashMap<usize, bool>,
}

#[derive(Clone, Debug)]
pub struct PrioritySemaphore {
    semaphore: Arc<Semaphore>,
    interleave_index: Arc<AtomicUsize>,
    wait_list: Arc<tokio::sync::Mutex<PrioritySemaphoreWaitList>>,
    priority: Arc<Mutex<PriorityRange>>,
}

#[derive(Clone, Default, Debug)]
pub struct PrioritySemaphoreWaitList {
    wait_list: Weak<tokio::sync::Mutex<HashMap<usize, oneshot::Sender<OwnedSemaphorePermit>>>>,
}

impl PrioritySemaphore {
    pub fn new(permits: usize, index: usize, priority: Arc<Mutex<PriorityRange>>) -> Self {
        Self {
            semaphore: Arc::new(Semaphore::new(permits)),
            wait_list: Arc::new(Default::default()),
            interleave_index: Arc::new(AtomicUsize::new(index)),
            priority,
        }
    }

    #[instrument(skip(self))]
    pub async fn acquire(
        self,
        index: usize,
    ) -> Result<OwnedSemaphorePermit, tokio::sync::AcquireError> {
        let Self {
            semaphore,
            wait_list,
            priority,
            interleave_index,
        } = self;

        tracing::trace!("acquire {}", index);

        loop {
            let (tx, rx) = oneshot::channel();

            let wait_list_strong = {
                let mut wait_list_guard = wait_list.lock().await;
                match wait_list_guard.wait_list.upgrade() {
                    Some(wait_list) => {
                        wait_list.lock().await.insert(index, tx);
                        None
                    }
                    None => {
                        let wait_list_strong = Arc::new(tokio::sync::Mutex::new(HashMap::new()));
                        wait_list_guard.wait_list = Arc::downgrade(&wait_list_strong);
                        wait_list_strong.lock().await.insert(index, tx);
                        Some(wait_list_strong)
                    }
                }
            };

            if let Some(wait_list_strong) = wait_list_strong {
                tracing::trace!("waiting at {}", index);
                let permit = semaphore.clone().acquire_owned().await;
                match permit {
                    Ok(mut permit) => {
                        tracing::trace!("getting priority {}", index);
                        let interleave = interleave_index.load(Ordering::Relaxed);
                        let pr = priority
                            .lock()
                            .clone()
                            .interleave_at(interleave)
                            .collect_vec();
                        tracing::trace!(
                            "priorities from {}: {:?} within {:?}",
                            interleave,
                            pr,
                            priority.lock().range
                        );

                        for i in pr {
                            tracing::trace!("try sending to {}", i);
                            // tracing::trace!("locking wait_list");
                            let mut wait_list = wait_list_strong.lock().await;
                            // tracing::trace!("wait_list locked");
                            let entry = wait_list.entry(i);

                            let entry = match entry {
                                std::collections::hash_map::Entry::Occupied(entry) => entry,
                                std::collections::hash_map::Entry::Vacant(_) => continue,
                            };

                            tracing::trace!("sending permit to {}", i);
                            match entry.remove().send(permit) {
                                Ok(_) => break,
                                Err(val) => permit = val,
                            };
                        }
                    }

                    Err(err) => {
                        return Err(err);
                    }
                }
            }

            tracing::trace!("waiting for permit {}", index);
            if let Ok(permit) = rx.await {
                tracing::trace!("received permit: {}", index);
                return Ok(permit);
            }
        }
    }
}

impl PriorityRange {
    pub fn new(range: Range<usize>) -> Self {
        Self {
            range,
            filtered: Default::default(),
        }
    }

    pub fn filter(&mut self, filter: usize) {
        *self.filtered.entry(filter).or_default() = true;
    }

    pub fn interleave_at(&mut self, index: usize) -> impl Iterator<Item = usize> + '_ {
        crate::split_and_interleave_at(self.range.clone(), index)
    }
}

pub trait Wait: Sized {
    type Output: std::future::Future;

    // output of this function should be treated as a permit.
    fn wait(&mut self) -> Self::Output;
}

pub struct WaitFn<F> {
    inner: F,
}

impl<F, R> Wait for WaitFn<F>
where
    F: FnMut() -> R,
    R: std::future::Future,
{
    type Output = R;

    fn wait(&mut self) -> Self::Output {
        (self.inner)()
    }
}

pub fn wait_fn<F, R>(waiter: F) -> WaitFn<F>
where
    F: FnMut() -> R,
    R: std::future::Future,
{
    WaitFn { inner: waiter }
}

pub struct ExplorerEntryLoader<W: Wait> {
    pub path: PathBuf,
    pub waiter: W,
    pub setting: Arc<Mutex<ExplorerEntryLoaderSetting>>,
    pub cache: ExplorerLoaderCache,
}

#[derive(Clone, Debug, PartialEq, Eq, Serialize, Deserialize, Derivative)]
#[derivative(Default)]
#[serde(default)]
pub struct ExplorerEntryLoaderSetting {
    pub filter: FilterType,
    #[derivative(Default(value = "(300,300)"))]
    pub max_resize: (u32, u32),
}

pub fn sha_path(path: &std::path::Path) -> String {
    let sha = sha2::Sha256::digest(path.to_string_lossy().as_bytes());

    format!("{:x}", sha)
}

#[derive(Clone, Default, Debug, Serialize, Deserialize)]
pub struct ExplorerLoaderCache {
    pub map: Arc<Mutex<HashMap<String, String>>>,
    #[serde(default)]
    pub list: Arc<Mutex<HashMap<String, HashSet<String>>>>,
}

impl ExplorerLoaderCache {
    pub fn get_from_sha(&self, sha: &String) -> Option<String> {
        self.map.lock().get(sha).cloned()
    }

    pub fn get_from_path(&self, path: &std::path::Path) -> Option<String> {
        self.get_from_sha(&sha_path(path))
    }

    pub fn insert_sha(&self, path: &std::path::Path, target: String) {
        log::debug!("insert cache for {} using {}", path.display(), target);
        self.map.lock().insert(sha_path(path), target.clone());
        self.list
            .lock()
            .entry(target)
            .or_default()
            .insert(path.display().to_string());
    }

    pub fn insert_sha_recursive(
        &self,
        root: &std::path::Path,
        mut path: &std::path::Path,
        target: &str,
    ) {
        let root = root.to_path_buf();

        while let Some(parent) = path.parent() {
            let should_break = Some(&root) == path.canonicalize().as_ref().ok();

            let is_root_file = || root.extension().is_some();
            // let is_root_file = || matches!(root.as_ref(), Ok(x) if x.extension().is_some());

            // if not should break should always insert
            // and it should also insert if root is file
            // it would'nt cache archive file otherwise.
            let should_insert = !should_break || is_root_file();

            if should_insert {
                self.insert_sha(path, target.to_string());
                path = parent;
            }

            if should_break {
                break;
            }
        }
    }
}

impl<W> ExplorerEntryLoader<W>
where
    W: Wait,
    W::Output: std::future::Future,
{
    pub fn new(
        path: PathBuf,
        waiter: W,
        setting: Arc<Mutex<ExplorerEntryLoaderSetting>>,
        cache: ExplorerLoaderCache,
    ) -> Self {
        Self {
            path,
            waiter,
            setting,
            cache,
        }
    }

    pub async fn search(mut self) -> Option<crate::image::EguiSplittedImageData> {
        if self.path.exists() {
            let dir = walkdir::WalkDir::new(&self.path)
                .same_file_system(true)
                .sort_by(|a, b| crate::path::compare_natural(a.file_name(), b.file_name()));

            let dir = dir.into_iter().filter_map(|it| it.ok());

            let dir = futures::stream::unfold(dir, |mut dir| async {
                tokio::task::spawn_blocking(|| {
                    let next = dir.next();

                    next.map(|next| (next, dir))
                })
                .await
                .ok()
                .unwrap()
            });
            futures::pin_mut!(dir);

            while let Some(entry) = dir.next().await {
                let entry: walkdir::DirEntry = entry;

                if let Some(image) = self.load_path(entry.path().to_path_buf()).await {
                    return Some(image);
                }
            }
        }

        None
    }

    async fn load_path(&mut self, path: PathBuf) -> Option<crate::image::EguiSplittedImageData> {
        let time = Instant::now();
        log::debug!("opening {:?}", path);
        let mut image = None;

        let _wait = self.waiter.wait().await;
        let mut use_cache = false;

        let sha_path = |path: &std::path::Path| {
            let sha = sha2::Sha256::digest(path.to_string_lossy().as_bytes());

            format!("{:x}", sha)
        };

        let cache_dir =
            std::env::var("XDG_CACHE_HOME").unwrap_or_else(|_| std::env::var("HOME").unwrap());
        let cache_dir = PathBuf::from(cache_dir).join("fmr");

        let (sha, cache) = {
            let sha = sha_path(&path);
            let cache = self
                .cache
                .get_from_sha(&sha)
                .map(|it| (cache_dir.join(&it), it));

            match cache.map(|(it, sha)| (it.exists(), sha, it)) {
                Some((true, sha, it)) => {
                    log::debug!("using cache from {}", it.display());
                    (sha, it)
                }
                _ => {
                    log::debug!(
                        "no cache found for {} from {}",
                        sha,
                        self.cache.map.lock().len()
                    );
                    let sha = sha_path(&path);
                    let cache = cache_dir.join(&sha);

                    (sha, cache)
                }
            }
        };

        if image.is_none() && cache.exists() {
            if let Ok(reader) = crate::image::Reader::open(&cache) {
                image = reader
                    .into_frames()
                    .into_collector()
                    .collect_as_option_vector()
                    .await;

                use_cache = image.is_some();
            }
        }

        if image.is_none() {
            if let Ok(reader) = crate::image::Reader::open(&path) {
                image = reader
                    .into_frames()
                    .into_collector()
                    .collect_as_option_vector()
                    .await;
            }
        }

        let image = match image {
            Some(image) => image,
            None => self.search_file(path.clone()).await?,
        };

        let size = self.setting.lock().max_resize;
        let filter = self.setting.lock().filter.to_image();

        log::trace!("loading image {:?} in {:?}", path, time.elapsed());

        yield_now().await;
        let image = image.resize(size.0, size.1, filter);
        log::trace!("resize image {:?} in {:?}", path, time.elapsed());

        // doesn't need to update if already using cache
        if !use_cache && std::fs::create_dir_all(cache_dir).is_ok() {
            if let Ok(mut file) = std::fs::File::create(cache) {
                if let Err(err) = image.write_to(&mut file, image::ImageFormat::Jpeg) {
                    log::error!("error writing cache: {}", err);
                }
            }
        }

        self.cache.insert_sha_recursive(&self.path, &path, &sha);
        log::trace!(
            "insert sha recursive into [{:?}] [{:?}] [{}] in {:?}",
            self.path,
            path,
            sha,
            time.elapsed()
        );

        yield_now().await;
        let image = image.into_allocatable(size);
        log::trace!("splitting image {:?} in {:?}", path, time.elapsed());

        yield_now().await;
        let image = image.into_egui();
        log::trace!("into egui {:?} in {:?}", path, time.elapsed());

        Some(image)
    }

    async fn search_file(&mut self, path: PathBuf) -> Option<crate::image::ImageData> {
        let file = std::fs::File::open(&path).ok()?;
        if let Ok(mut zip) = ZipArchive::new(file) {
            for i in 0..zip.len() {
                if let Some(image) = crate::tools::zip::load_image(|| zip.by_index(i)).await {
                    return Some(image);
                }
            }
        }

        #[cfg(feature = "libarchive")]
        if crate::tools::archive::can_read(&path) {
            if let Some(image) = self.search_image_archive(path).await {
                return Some(image);
            }
        }

        None
    }

    #[cfg(feature = "libarchive")]
    async fn search_image_archive(&mut self, path: PathBuf) -> Option<crate::image::ImageData> {
        let it = {
            let reader = crate::tools::archive::open(&path)
                .ok()?
                .into_iter()
                .filter_map(|it| it.ok())
                .find(|it| crate::tools::archive::is_image(it));

            crate::tools::archive::load_image_from_opt_entry(reader)
        };

        it.await
    }
}
