use std::{path::PathBuf, sync::Arc};

use futures::stream::StreamExt;
use parking_lot::RwLock;
use serde::{Deserialize, Serialize};
use tokio::sync::{watch, Semaphore};

use crate::{
    explorer::{
        Explorer, ExplorerLoader, ExplorerLoaderCache, ExplorerLoaderSetting, ExplorerOutput,
        ExplorerSetting, ExplorerView, PathExplorerItem,
    },
    image_search::search_image,
    AbortOnDropHandle,
};

pub struct AppExplorer {
    path: PathBuf,
    inner: Arc<RwLock<Explorer<PathExplorerItem>>>,
    // previous output used to handle event
    prev_output: Option<ExplorerOutput>,
    setting_sender: watch::Sender<ExplorerLoaderSetting>,
    #[allow(dead_code)]
    handle: AbortOnDropHandle<()>,
}

#[derive(Clone, Serialize, Deserialize, Debug, Default)]
#[serde(default)]
pub struct AppExplorerSetting {
    pub explorer: ExplorerSetting,
    pub loader: ExplorerLoaderSetting,
    #[serde(skip)]
    pub cache: ExplorerLoaderCache,
}

impl AppExplorer {
    pub fn new(
        path: PathBuf,
        setting: AppExplorerSetting,
        selected_entry: Option<PathBuf>,
        ctx: egui::Context,
    ) -> Self {
        let explorer = Arc::new(RwLock::new(Explorer::default()));
        let (setting_sender, setting_receiver) = watch::channel(setting.loader);

        let loader = ExplorerLoader {
            explorer: explorer.clone(),
            path: path.clone(),
            selected_entry,
            setting_receiver,
            cache: setting.cache,
            ctx,
        };

        let handle = tokio::spawn(loader.load());

        Self {
            path,
            inner: explorer,
            prev_output: None,
            setting_sender,
            handle: AbortOnDropHandle(handle),
        }
    }

    pub async fn load_path(
        explorer: Arc<RwLock<Explorer<PathExplorerItem>>>,
        path: PathBuf,
        selected_entry: Option<PathBuf>,
        ctx: egui::Context,
    ) {
        let mut dir = match tokio::fs::read_dir(&path).await {
            Ok(dir) => dir,
            _ => return,
        };

        {
            let mut content = vec![];

            while let Ok(Some(entry)) = dir.next_entry().await {
                content.push(entry);
            }

            content.sort_by(|a, b| crate::path::compare_natural(&a.file_name(), &b.file_name()));

            explorer.write().content.reserve(content.len());

            let child = futures::stream::FuturesUnordered::new();
            let semaphore = Arc::new(Semaphore::new(5));
            let size = ctx.input().max_texture_side;

            let default_texture = image::DynamicImage::new_rgb8(1, 1);
            let default_texture = crate::image::ImageData::StaticImage(default_texture)
                .into_allocatable((size as u32, size as u32))
                .into_egui()
                .alloc(ctx.tex_manager(), format!("{:?} default", path));
            // .into_view_state();

            for (i, entry) in content.into_iter().enumerate() {
                if let Some(it) = PathExplorerItem::new(entry.path()) {
                    explorer.write().content.push(it);

                    let explorer_ = explorer.clone();
                    let path = entry.path();
                    let ctx = ctx.clone();
                    let semaphore = semaphore.clone();
                    let default_texture = default_texture.clone();
                    child.push(crate::spawn_and_abort_on_drop(async move {
                        let explorer = explorer_;
                        let _permit = match semaphore.clone().acquire_owned().await {
                            Ok(permit) => permit,
                            _ => return,
                        };

                        // let loader = L
                        let image = search_image(ctx.clone(), path.clone()).await;

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

            futures::pin_mut!(child);
            while (child.next().await).is_some() {
                tokio::task::yield_now().await;
            }
        }
        //
    }

    /// Get a reference to the app explorer's path.
    #[must_use]
    pub fn path(&self) -> &PathBuf {
        &self.path
    }

    pub fn handle_event(
        &mut self,
        mut on_open: Option<impl AppExplorerOnOpen>,
        event: &egui::Event,
    ) -> bool {
        if let egui::Event::Key {
            key: egui::Key::Enter,
            pressed: true,
            modifiers,
        } = event
        {
            let inner = self.inner.read();
            let item = inner.current_item();

            if let Some(item) = item {
                return if modifiers.is_none() {
                    on_open.as_mut().map(|it| it.on_open(item))
                } else if modifiers.shift_only() {
                    on_open.as_mut().map(|it| it.on_open_with_shift(item))
                } else {
                    Some(false)
                }
                .unwrap_or(false);
            }
        }

        if let Some(output) = &self.prev_output {
            if ExplorerView::handle_key(&mut self.inner.write(), output, event) {
                return true;
            }
        }

        false
    }
}

pub trait AppExplorerOnOpen {
    fn on_open(&mut self, explorer: &PathExplorerItem) -> bool;
    fn on_open_with_shift(&mut self, explorer: &PathExplorerItem) -> bool;
}

pub struct AppExplorerView<'a, OnOpen> {
    explorer: &'a mut AppExplorer,
    setting: &'a AppExplorerSetting,
    on_open: Option<OnOpen>,
}

impl<'a, OnOpen: AppExplorerOnOpen> AppExplorerView<'a, OnOpen> {
    pub fn new(explorer: &'a mut AppExplorer, setting: &'a AppExplorerSetting) -> Self {
        Self {
            explorer,
            setting,
            on_open: None,
        }
    }

    pub fn on_open(mut self, on_open: OnOpen) -> Self {
        self.on_open = Some(on_open);
        self
    }

    pub fn show(self, ui: &mut egui::Ui) -> egui::Response {
        let Self {
            explorer,
            setting,
            mut on_open,
        } = self;

        let response = ui
            .scope(|ui| {
                ui.centered_and_justified(|ui| {
                    explorer.prev_output = None;

                    let output = ExplorerView::new(&mut explorer.inner.write(), &setting.explorer)
                        .id_source("explorer")
                        .show(ui);

                    let inner = explorer.inner.read();
                    let item = inner.current_item();
                    if let Some(item) = item {
                        if output.is_double_clicked {
                            on_open.as_mut().map(|it| it.on_open(item));
                        }
                    }
                    explorer.prev_output = Some(output);
                });
            })
            .response;

        let index = explorer.inner.read().current_index();

        let setting = ExplorerLoaderSetting {
            index,
            ..setting.loader.clone()
        };

        if setting != *explorer.setting_sender.borrow() {
            explorer.setting_sender.send(setting).ok();
        }

        if response.gained_focus() {
            ui.memory().lock_focus(response.id, true);
        }

        response
    }
}
