use std::{
    io::Read,
    ops::Range,
    path::PathBuf,
    sync::{Arc, Weak},
};

use futures::{Future, FutureExt};
use parking_lot::{Mutex, RwLock};
use tokio::sync::{watch, OwnedSemaphorePermit, Semaphore};

use crate::{
    image::{EguiSplittedImageData, ImageData, LoadingTexture, TextureViewState},
    reader::Reader,
    AbortOnDropHandle,
};

pub struct ReaderLoader {
    pub path: PathBuf,
    pub reader: Arc<RwLock<Reader>>,
    pub ctx: egui::Context,
    pub setting_receiver: watch::Receiver<ReaderLoaderSetting>,
}

#[derive(PartialEq, Eq, Clone, Copy, Debug)]
pub struct ReaderLoaderSetting {
    pub index: usize,
    pub preload_prev: usize,
    pub preload_next: usize,
}

#[derive(Clone)]
struct LoaderEntry {
    opener_index: usize,
    name: String,
    item: Weak<Mutex<TextureViewState>>,
    handle: Arc<Mutex<Option<AbortOnDropHandle<()>>>>,
}

impl ReaderLoaderSetting {
    pub fn range(&self, max: usize) -> Range<usize> {
        let min = self
            .index
            .checked_sub(self.preload_prev)
            .unwrap_or(0)
            .clamp(0, (self.index).min(max));

        let max = self
            .index
            .checked_add(self.preload_next)
            .unwrap_or(max)
            .clamp((self.index + 1).min(max), max);

        min..max
    }
}

impl ReaderLoader {
    pub async fn load(self) {
        let open_path = self.path.clone();
        if open_path.exists() {
            let path = if open_path.is_file()
                && image::ImageFormat::from_path(&open_path)
                    .map(|it| it.can_read())
                    .unwrap_or(false)
            {
                open_path.parent().unwrap().to_path_buf()
            } else {
                open_path.clone()
            };
            log::info!("opening {:?} as {:?}", path, open_path);

            if path.is_file() {
                self.load_file(path).await;
            } else if path.is_dir() {
                self.load_folder(path).await;
            };
        }
    }

    pub async fn load_folder(self, path: PathBuf) {
        let mut dir = match tokio::fs::read_dir(path).await {
            Ok(dir) => dir,
            Err(_) => return,
        };

        let mut entries = vec![];
        while let Ok(Some(it)) = dir.next_entry().await {
            let can_read = ImageData::can_read(&it.file_name().to_string_lossy());

            if can_read {
                entries.push(it);
            }
        }

        entries.sort_by(|a, b| crate::path::compare_natural(&a.file_name(), &b.file_name()));
        let map = entries
            .iter()
            .enumerate()
            .map(|(i, it)| (i, it.path().to_string_lossy().to_string()))
            .collect();

        self.schedule(map, move |index| {
            let it = entries[index].path();
            async move {
                let image_reader = crate::image::Reader::open(it);
                if let Ok(image_reader) = image_reader {
                    image_reader
                        .into_frames()
                        .into_collector()
                        .collect_as_option_vector()
                        .await
                } else {
                    None
                }
            }
        })
        .await;
    }

    pub async fn load_file(self, path: PathBuf) {
        let file = match std::fs::File::open(path) {
            Ok(file) => file,
            Err(_) => return,
        };

        if let Ok(zip) = zip::ZipArchive::new(file) {
            self.load_zip(zip).await
        }
    }

    pub async fn load_zip<R: Read + std::io::Seek + 'static>(self, mut zip: zip::ZipArchive<R>) {
        let names = zip
            .file_names()
            .filter(|it| ImageData::can_read(it))
            .map(|it| it.to_string())
            .collect::<Vec<_>>();

        let map = names.iter().cloned().enumerate().collect();

        self.schedule(map, move |index| {
            let name = names[index].clone();

            crate::tools::zip::load_image_with_ref(&mut zip, |zip| {
                let name = name;
                zip.by_name(&name)
            })
        })
        .await;
    }

    pub async fn schedule<F, R>(mut self, mut map: Vec<(usize, String)>, mut opener: F)
    where
        F: FnMut(usize) -> R,
        R: Future<Output = Option<ImageData>> + Send + 'static,
    {
        // let index= reader.
        map.sort_by(|(_, a), (_, b)| natord::compare_ignore_case(&a, &b));
        let mut entries = vec![];

        self.reader.write().images.clear();
        for (opener_index, name) in map {
            let texture = LoadingTexture::new(name.clone(), None);
            let item = Arc::new(Mutex::new(TextureViewState::Loading(texture)));

            let texture = TextureViewState::Mutable(item.clone());

            self.reader.write().images.push(texture);
            entries.push(LoaderEntry {
                opener_index,
                name,
                item: Arc::downgrade(&item),
                handle: Arc::new(Mutex::new(None)),
            });
        }

        let semaphore = Arc::new(Semaphore::new(2));
        let mut setting_receiver = self.setting_receiver.clone();
        // let Self {
        //     path: _,
        //     reader: _,
        //     ctx,
        //     setting_receiver,
        // } = self;

        loop {
            let spawner = self.spawn(&entries, &mut opener, semaphore.clone());
            let spawner = spawner.fuse();
            let waiter = setting_receiver.changed().fuse();
            futures::pin_mut!(spawner);
            futures::pin_mut!(waiter);

            let changed = futures::select! {
                changed = waiter => {
                    changed
                }
                _ = spawner => {
                    waiter.await
                }
            };

            if let Err(_) = changed {
                break;
            }
        }
    }

    async fn spawn<F, R>(
        &mut self,
        entries: &Vec<LoaderEntry>,
        opener: &mut F,
        semaphore: Arc<Semaphore>,
    ) where
        F: FnMut(usize) -> R,
        R: Future<Output = Option<ImageData>> + Send + 'static,
    {
        let Self {
            path: _,
            reader: _,
            ctx,
            setting_receiver,
        } = self;

        let setting = *setting_receiver.borrow();
        let preload = setting.range(entries.len());
        log::info!("paged changed to {:?}, preload {:?} page", setting, preload);

        let mut query = |index: usize, permit: Option<OwnedSemaphorePermit>| {
            let entry = match entries.get(index) {
                Some(entry) => entry,
                None => return,
            };

            let item = match (preload.contains(&index), entry.item.upgrade()) {
                (true, Some(item)) => item,
                // remove handle from item that isnt in preload or the item is None.
                (_, item) => {
                    *entry.handle.lock() = None;

                    // make item that is some loading again.
                    if let Some(item) = item {
                        item.lock().make_loading();
                    }

                    return;
                }
            };

            let loader = || {
                log::info!("loading {} {}", entry.name, entry.opener_index);
                let entry = entry.clone();
                let fut = opener(entry.opener_index);
                let ctx = ctx.clone();

                async move {
                    let _permit = permit;
                    log::info!("acquired permit for {} {}", entry.name, entry.opener_index);
                    let texture = Self::load_texture(entry.name, ctx.clone(), fut).await;
                    if let Some(texture) = texture {
                        if let Some(item) = entry.item.upgrade() {
                            *item.lock() = texture.into();
                            ctx.request_repaint();
                        }
                    }

                    *entry.handle.lock() = None;
                }
            };

            let mut handle = entry.handle.lock();
            if handle.is_none() {
                if item.lock().is_loading() {
                    *handle = Some(crate::spawn_and_abort_on_drop(loader()));
                }
            }
        };

        // query item that isn't in preload first.
        for i in (0..preload.start).chain(preload.end..entries.len()) {
            query(i, None);
        }

        let preload = crate::split_and_interleave_at(preload.clone(), setting.index);
        // let preload = {
        //     let index = setting.index;
        //     (index..preload.end).interleave((preload.start..index).rev())
        // };

        for i in preload {
            if let Ok(permit) = semaphore.clone().acquire_owned().await {
                query(i, Some(permit));
            }
        }
    }

    pub async fn load_texture(
        name: String,
        ctx: egui::Context,
        fut: impl Future<Output = Option<ImageData>>,
    ) -> Option<crate::image::TextureHandle> {
        let time = std::time::Instant::now();
        let image = fut.await?;
        let name = name;
        let max_size = ctx.input().max_texture_side as u32;

        tokio::task::yield_now().await;
        let image = image.into_allocatable((max_size, max_size));
        log::debug!("splitting {:?} in {:?}", name, time.elapsed());

        tokio::task::yield_now().await;
        let image = EguiSplittedImageData::from(image);
        log::debug!("into egui {:?} in {:?}", name, time.elapsed());

        tokio::task::yield_now().await;
        let texture =
            crate::image::TextureHandle::from_image(ctx.tex_manager(), name.clone(), image);

        Some(texture)
    }
}
