use std::{
    path::{Path, PathBuf},
    sync::{atomic::AtomicBool, Arc},
};

use parking_lot::{RwLock, RwLockReadGuard};
use serde::{Deserialize, Serialize};
use tokio::sync::watch;

use crate::{
    image::TextureOption,
    reader::{
        loader::ReaderLoaderSetting, PagedReaderState, Reader, ReaderMode, ReaderModeState,
        ReaderSetting, ReaderView,
    },
    AbortOnDropHandle, Vec2Ext,
};

#[derive(Clone, PartialEq, Eq, Serialize, Deserialize, Default, Debug)]
#[serde(default)]
pub struct AppReaderSetting {
    pub reader: ReaderSetting,
    pub change_folder_with_scroll_wheel: bool,
    pub preload_prev: usize,
    pub preload_next: usize,
    #[serde(default)]
    pub texture_option: TextureOption,

    #[serde(skip)]
    pub folder_sorter: AppReaderFolderSorter,
}

#[derive(Clone)]
pub struct AppReaderFolderSorter(
    pub Arc<dyn (Fn(&Path, &Path) -> std::cmp::Ordering) + Send + Sync>,
);

impl PartialEq for AppReaderFolderSorter {
    fn eq(&self, other: &Self) -> bool {
        Arc::ptr_eq(&self.0, &other.0)
    }
}

impl Eq for AppReaderFolderSorter {}

impl Default for AppReaderFolderSorter {
    fn default() -> Self {
        Self(Arc::new(|a, b| {
            crate::path::compare_natural(a.file_name().unwrap(), b.file_name().unwrap())
        }))
    }
}

impl AppReaderFolderSorter {
    pub fn replace(
        &mut self,
        fun: impl Fn(&Path, &Path) -> std::cmp::Ordering + Send + Sync + 'static,
    ) {
        self.0 = Arc::new(fun);
    }
}

impl std::fmt::Debug for AppReaderFolderSorter {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        f.debug_tuple("AppReaderFolderSorter").finish()
    }
}

pub struct AppReader {
    pub path: PathBuf,
    pub setting: AppReaderSetting,
    reader: Arc<RwLock<Reader>>,

    index_sender: watch::Sender<ReaderLoaderSetting>,
    is_done_initial_loading: Arc<AtomicBool>,
    #[allow(dead_code)]
    handle: AbortOnDropHandle<()>,
}

impl AppReader {
    pub fn new(path: PathBuf, setting: AppReaderSetting, ctx: egui::Context) -> Self {
        let images = Vec::new();

        let mode = match setting.reader.mode {
            ReaderMode::Paged => {
                let mut paged = PagedReaderState::default();
                paged.read_from_right = setting.reader.paged.read_from_right;
                paged.reset(1);
                ReaderModeState::Paged(paged)
            }
            ReaderMode::Vertical => crate::reader::ReaderModeState::Vertical(Default::default()),
        };

        let reader = Arc::new(RwLock::new(Reader::new(images, mode)));

        let current_index = ReaderLoaderSetting {
            index: 0,
            preload_next: setting.preload_next,
            preload_prev: setting.preload_prev,
            texture_option: setting.texture_option,
        };
        let (index_sender, index_receiver) = watch::channel(current_index);
        index_sender.send(current_index).ok();

        log::info!("open reader in {:?}", path);

        let is_done_initial_loading = Arc::new(AtomicBool::new(false));

        let loader = crate::reader::loader::ReaderLoader {
            reader: reader.clone(),
            path: path.clone(),
            ctx,
            setting_receiver: index_receiver,
            is_done_initial_loading: is_done_initial_loading.clone(),
        };

        let handle = tokio::spawn(loader.load());

        Self {
            path,
            reader,
            setting,
            index_sender,
            is_done_initial_loading,
            handle: AbortOnDropHandle(handle),
        }
    }

    pub fn open(&mut self, path: PathBuf, ctx: egui::Context) {
        *self = Self::new(path, self.setting.clone(), ctx);
    }

    pub fn reader(&self) -> RwLockReadGuard<'_, Reader> {
        self.reader.read()
    }

    /// Get a reference to the app reader's path.
    #[must_use]
    pub fn path(&self) -> &PathBuf {
        &self.path
    }

    pub fn change_folder(&mut self, direction: isize, ctx: &egui::Context) -> bool {
        let path = crate::path::get_sorted_folder_by(self.path.clone(), direction, |a, b| {
            self.setting.folder_sorter.0(&a.path(), &b.path())
        });

        match path {
            Some(path) => {
                self.open(path, ctx.clone());
                true
            }
            _ => false,
        }
    }

    pub fn handle_event(
        &mut self,
        setting: &AppReaderSetting,
        ctx: &egui::Context,
        event: &egui::Event,
    ) -> bool {
        let Self {
            path: _,
            setting: _,
            reader,
            handle: _,
            index_sender: _,
            is_done_initial_loading: _,
        } = self;

        if reader.write().handle_event(event) {
            return true;
        }

        let is_vertical = reader.read().state.is_vertical();
        let read_from_right = setting.reader.paged.read_from_right;

        let mut change_folder = |direction: isize| self.change_folder(direction, ctx);

        let mut change_folder_with_key =
            |enable: bool, up: egui::Key, down: egui::Key, reverse: bool| {
                crate::key::handle_key(
                    enable,
                    event,
                    up,
                    down,
                    |it| it.is_none() || it.command_only() || it.shift_only(),
                    |direction| {
                        let multiplier = if reverse { -1 } else { 1 };

                        let direction = direction * multiplier;
                        change_folder(direction as isize)
                    },
                )
            };

        let mut handled = false;
        handled |= change_folder_with_key(true, egui::Key::ArrowUp, egui::Key::ArrowDown, false);

        handled |= change_folder_with_key(
            !is_vertical,
            egui::Key::ArrowLeft,
            egui::Key::ArrowRight,
            read_from_right,
        );

        handled
    }
}

pub struct AppReaderView<'a> {
    state: &'a mut AppReader,
    setting: &'a AppReaderSetting,
}

impl<'a> AppReaderView<'a> {
    pub fn new(state: &'a mut AppReader, setting: &'a AppReaderSetting) -> Self {
        Self { state, setting }
    }

    pub fn show(self, ui: &mut egui::Ui) -> egui::Response {
        let Self { setting, state } = self;
        let mut read_from_right = false;
        let mut is_vertical = false;

        let response = ui
            .centered_and_justified(|ui| {
                let mut reader = state.reader.write();
                // reader.setting = setting.reader.clone();
                read_from_right = reader.is_read_from_right();
                is_vertical = reader.state.is_vertical();

                ReaderView::new(&mut reader, &setting.reader).show(ui)
            })
            .inner;

        ui.input_mut().events.retain(|event| {
            if setting.change_folder_with_scroll_wheel && response.hovered() {
                if let egui::Event::Scroll(scroll) = event {
                    let mut step = scroll.to_step();
                    step[0] *= !is_vertical as isize;
                    step[0] *= if read_from_right { -1 } else { 1 };

                    if state
                        .is_done_initial_loading
                        .load(std::sync::atomic::Ordering::Relaxed)
                    {
                        match step {
                            [0, i] | [i, 0] if i != 0 => return !state.change_folder(i, ui.ctx()),
                            _ => {}
                        };
                    }
                }
            }

            true
        });

        if let ReaderModeState::Paged(paged) = &state.reader().state {
            let current = ReaderLoaderSetting {
                index: paged.index,
                preload_prev: setting.preload_prev,
                preload_next: setting.preload_next,
                texture_option: setting.texture_option,
            };
            if *state.index_sender.borrow() != current {
                state.index_sender.send(current).ok();
            }
        } else {
            let current = ReaderLoaderSetting {
                index: 0,
                preload_prev: usize::MAX,
                preload_next: usize::MAX,
                texture_option: setting.texture_option,
            };
            if *state.index_sender.borrow() != current {
                state.index_sender.send(current).ok();
            }
        }

        if response.gained_focus() {
            ui.memory().lock_focus(response.id, true);
        }

        response
    }
}
