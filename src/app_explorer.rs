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
