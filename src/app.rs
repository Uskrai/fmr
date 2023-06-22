use std::path::{Path, PathBuf};

use egui::widgets::DragValue;
use serde::{Deserialize, Serialize};

use crate::{
    explorer::{ExplorerLoaderCache, FilterType, PathExplorerItem},
    inspection::DebugUI,
    path::{path_serde, PathSorterSetting},
    reader::ReaderMode,
    AppExplorer, AppExplorerOnOpen, AppExplorerSetting, AppExplorerView, AppReader,
    AppReaderSetting, AppReaderView,
};

#[derive(Default, Serialize, Deserialize, Debug)]
#[serde(default)]
pub struct AppSetting {
    #[serde(with = "path_serde")]
    path: PathBuf,
    reader: AppReaderSetting,
    explorer: AppExplorerSetting,
    path_sorter: PathSorterSetting,
}

pub struct App {
    mode: Option<AppMode>,
    context: egui::Context, // context used to request repaint
    setting: AppSetting,
    reading_progress: crate::ReadingProgress,
    setting_storage: Option<crate::storage::FSStorage>,
    cache_storage: Option<crate::storage::FSStorage>,
    data_storage: Option<crate::storage::FSStorage>,
    // path: PathBuf,
    // reader_option: ReaderOption,
    tokio_runtime: tokio::runtime::Runtime,
    debug_ui: DebugUI,
}

impl Drop for App {
    fn drop(&mut self) {
        tracing::info!("exiting apps");
    }
}

#[derive(Debug, Default, Serialize, Deserialize)]
pub struct AppCache {
    explorer_cache: ExplorerLoaderCache,
}

#[derive(Debug, Default, Serialize, Deserialize)]
pub struct AppData {
    pub reading_progress: crate::ReadingProgress,
}

pub enum AppMode {
    Explorer(AppExplorer),
    Reader(AppReader),
}

impl AppMode {
    pub fn path(&self) -> &Path {
        match self {
            AppMode::Explorer(explorer) => explorer.path(),
            AppMode::Reader(reader) => reader.path(),
        }
    }
}

impl App {
    pub fn new(context: &eframe::CreationContext, tokio_runtime: tokio::runtime::Runtime) -> Self {
        crate::setup_custom_fonts(&context.egui_ctx);

        let profile = std::env::var("FMR_PROFILE").unwrap_or_else(|_| "default".to_string());
        let setting_storage = crate::storage::FSStorage::prepare("fmr", &profile, "config.ron");

        let mut setting: AppSetting = match &setting_storage {
            Some(s) => ron::from_str(s.get_content()).unwrap_or_default(),
            None => Default::default(),
        };

        let cache_storage = crate::storage::FSStorage::prepare("fmr", "", "cache.ron");
        let data_storage = crate::storage::FSStorage::prepare("fmr", "", "data.ron");

        let cache: AppCache = match &cache_storage {
            Some(s) => ron::from_str(s.get_content()).unwrap_or_default(),
            None => Default::default(),
        };

        let data: AppData = match &data_storage {
            Some(s) => ron::from_str(s.get_content()).unwrap_or_default(),
            None => Default::default(),
        };

        setting.explorer.cache = cache.explorer_cache;
        let sorter = setting.path_sorter.subscribe();
        setting
            .explorer
            .sorter
            .replace(move |a, b| sorter.compare_path(a, b));

        let sorter = setting.path_sorter.subscribe();
        setting
            .reader
            .folder_sorter
            .replace(move |a, b| sorter.compare_path(a, b));

        if let Some(s) = context.storage {
            if let Some(s) = s.get_string("style") {
                let style = ron::from_str(&s);
                if let Ok(style) = style {
                    context.egui_ctx.set_style(std::sync::Arc::new(style));
                }
            }
        }

        Self {
            mode: Default::default(),
            setting,
            reading_progress: data.reading_progress,
            setting_storage,
            cache_storage,
            data_storage,
            // path,
            context: context.egui_ctx.clone(),
            // reader_option,
            debug_ui: Default::default(),
            tokio_runtime,
        }
    }

    pub fn context(&self) -> egui::Context {
        self.context.clone()
    }

    pub fn open_file_dialog(&mut self) {
        let dialog = rfd::FileDialog::new();

        let dialog = match &self.mode {
            Some(AppMode::Reader(reader)) => dialog.set_directory(&reader.path),
            _ => dialog,
        };

        if let Some(path) = dialog.pick_file() {
            self.open_reader(path.parent().unwrap().to_path_buf());
        }
    }

    pub fn open(&mut self, path: &Path, select_entry: Option<PathBuf>) {
        if path.exists() {
            if path.is_file() {
                self.open_reader(path.to_path_buf());
            } else if path.is_dir() {
                self.open_explorer(path.to_path_buf(), select_entry);
            }
        }
    }

    pub fn open_reader(&mut self, path: PathBuf) {
        let reader = AppReader::new(
            path,
            self.setting.reader.clone(),
            self.reading_progress.clone(),
            self.context(),
        );
        self.mode = Some(AppMode::Reader(reader));
    }

    pub fn open_explorer(&mut self, path: PathBuf, select_entry: Option<PathBuf>) {
        let explorer = AppExplorer::new(
            path,
            self.setting.explorer.clone(),
            select_entry,
            self.context(),
        );

        self.mode = Some(AppMode::Explorer(explorer));
    }
}

impl eframe::App for App {
    fn save(&mut self, _storage: &mut dyn eframe::Storage) {
        if let Some(storage) = self.setting_storage.as_mut() {
            let setting = ron::ser::to_string_pretty(&self.setting, Default::default()).unwrap();
            storage.set_content(setting);
            storage.flush();
        }

        if let Some(storage) = self.cache_storage.as_mut() {
            let cache = AppCache {
                explorer_cache: self.setting.explorer.cache.clone(),
            };

            storage
                .flush_content(move || ron::ser::to_string_pretty(&cache, Default::default()).ok());
        }

        if let Some(storage) = self.data_storage.as_mut() {
            let data = AppData {
                reading_progress: self.reading_progress.clone(),
            };

            storage
                .flush_content(move || ron::ser::to_string_pretty(&data, Default::default()).ok());
        }

        _storage.set_string("style", ron::ser::to_string(&self.context.style()).unwrap());
    }

    fn auto_save_interval(&self) -> std::time::Duration {
        std::time::Duration::from_secs(5)
    }

    fn update(&mut self, ctx: &egui::Context, _frame: &mut eframe::Frame) {
        egui::TopBottomPanel::top("top-panel").show(ctx, |ui| {
            egui::menu::bar(ui, |ui| {
                ui.menu_button("File", |ui| {
                    if ui.button("Open\tCtrl+O").clicked() {
                        self.open_file_dialog();
                    }
                });

                ui.menu_button("Explorer", |ui| {
                    let app_setting = &mut self.setting.explorer;
                    let setting = &mut app_setting.explorer;
                    ui.add(
                        egui::DragValue::new(&mut setting.minimum_column_width)
                            .prefix("Minimum Column width: "),
                    )
                    .on_hover_ui(|ui| {
                        ui.label("Set minimum column width of each items in explorer");
                    });

                    let setting = &mut app_setting.loader;

                    let setting = &mut setting.entry_setting;

                    ui.add(
                        egui::DragValue::new(&mut setting.max_resize.0)
                            .prefix("Max Resize Width: "),
                    );

                    ui.add(
                        egui::DragValue::new(&mut setting.max_resize.1)
                            .prefix("Max Resize Height: "),
                    );

                    ui.menu_button("Path Ordering", |ui| {
                        let setting = &mut self.setting.path_sorter;

                        ui.checkbox(setting.natural_mut(), "Natural");
                        ui.checkbox(setting.ascending_mut(), "Ascending");
                        ui.checkbox(setting.last_modified_mut(), "Last Modified");

                        setting.sync();
                    });

                    ui.menu_button("Scale Filter", |ui| {
                        let filter = &mut setting.filter;
                        ui.radio_value(filter, FilterType::Nearest, "Nearest");
                        ui.radio_value(filter, FilterType::Triangle, "Triangle");
                        ui.radio_value(filter, FilterType::CatmullRom, "CatmullRom");
                        ui.radio_value(filter, FilterType::Gaussian, "Gaussian");
                        ui.radio_value(filter, FilterType::Lanczos3, "Lanczos2");
                    });
                });

                ui.menu_button("Reader", |ui| {
                    let app_setting = &mut self.setting.reader;
                    let setting = &mut app_setting.reader;

                    ui.menu_button("Default Mode", |ui| {
                        let mode = &mut setting.mode;
                        ui.radio_value(mode, ReaderMode::Paged, "Paged");
                        ui.radio_value(mode, ReaderMode::Vertical, "Vertical");
                    });

                    ui.menu_button("Scale Option", |ui| {
                        let option = &mut setting.sizer;
                        ui.checkbox(option.fit_horizontal_mut(), "Fit Width");
                        ui.checkbox(option.fit_vertical_mut(), "Fit Height");
                        ui.checkbox(&mut option.shrink, "Shrink");
                        ui.checkbox(&mut option.enlarge, "Enlarge");

                        ui.add(
                            DragValue::new(&mut option.fit_to_scale)
                                .prefix("Scale: ")
                                .min_decimals(1)
                                .max_decimals(150),
                        );
                    });
                    ui.menu_button("Paged Option", |ui| {
                        let option = &mut setting.paged;
                        ui.checkbox(&mut option.read_from_right, "Read From Right");
                    });

                    ui.menu_button("Scroll", |ui| {
                        ui.checkbox(
                            &mut app_setting.change_folder_with_scroll_wheel,
                            "Change Folder with Scroll Wheel",
                        );

                        ui.add(
                            DragValue::new(&mut setting.scroll_per_page)
                                .prefix("Scroll per Page: "),
                        );

                        ui.add(
                            DragValue::new(&mut setting.scroll_per_arrow)
                                .prefix("Scroll per Arrow: "),
                        );
                    });

                    ui.menu_button("Loading", |ui| {
                        ui.add(
                            DragValue::new(&mut app_setting.preload_prev)
                                .prefix("Preload Previous image: ")
                                .clamp_range(usize::MIN..=usize::MAX),
                        );
                        ui.add(
                            DragValue::new(&mut app_setting.preload_next)
                                .prefix("Preload Next Image: ")
                                .clamp_range(usize::MIN..=usize::MAX),
                        );
                    });
                });

                ui.menu_button("Debug", |ui| {
                    self.debug_ui.show_menu(ui);
                });
            });
        });

        struct OnOpen<'a> {
            app: &'a mut App,
        }

        impl<'a> AppExplorerOnOpen for OnOpen<'a> {
            fn on_open(&mut self, it: &PathExplorerItem) -> bool {
                self.app.open(&it.path, None);
                true
            }

            fn on_open_with_shift(&mut self, it: &PathExplorerItem) -> bool {
                self.app.open_reader(it.path.clone());
                true
            }
        }

        let inner = egui::CentralPanel::default().show(ctx, |ui| match self.mode.take() {
            Some(AppMode::Explorer(mut explorer)) => {
                let setting = self.setting.explorer.clone();
                AppExplorerView::new(&mut explorer, &setting)
                    .on_open(OnOpen { app: self })
                    .show(ui);

                Some(AppMode::Explorer(explorer))
            }
            Some(AppMode::Reader(mut reader)) => {
                AppReaderView::new(&mut reader, &mut self.setting.reader).show(ui);

                Some(AppMode::Reader(reader))
            }
            None => None,
        });

        let response = inner.response.interact(egui::Sense::click_and_drag());
        let mut mode = inner.inner;
        let input_event_len = ctx.input(|i| i.events.len());

        if let (true, Some(mode)) = (response.hovered(), &mut mode) {
            ctx.input_mut(|i| {
                i.events.retain(|it| {
                    let mut handled = false;

                    handled |= match mode {
                        AppMode::Explorer(explorer) => {
                            explorer.handle_event(Some(OnOpen { app: self }), it)
                        }
                        AppMode::Reader(reader) => {
                            reader.handle_event(&self.setting.reader, ctx, it)
                        }
                    };

                    !handled
                })
            });
        }

        // if self.mode is Some then it is changed within the match block
        // so don't assign it again
        if self.mode.is_none() {
            self.mode = mode;
        }

        if let Some(AppMode::Reader(reader)) = &self.mode {
            if self.setting.path != reader.path {
                self.setting.path = reader.path.clone();
            }
        }

        if response.hovered() {
            ctx.input_mut(|input| {
                input.events.retain(|it| {
                    if let egui::Event::Key {
                        pressed: true,
                        key,
                        modifiers,
                        repeat: _,
                    } = it
                    {
                        if *key == egui::Key::O && modifiers.command_only() {
                            self.open_file_dialog();

                            return false;
                        }

                        if *key == egui::Key::Backspace {
                            let path = match &self.mode {
                                Some(mode) => mode.path().to_path_buf(),
                                None => self.setting.path.clone(),
                            };

                            // resolve symlink and make absolute when shift is pressed or path is relative
                            let path = if modifiers.shift_only() || path.is_relative() {
                                path.canonicalize().unwrap_or(path)
                            } else {
                                path
                            };

                            let parent = path
                                .parent()
                                .map(|it| it.to_path_buf())
                                .unwrap_or_else(|| "./".into());

                            let parent = if parent.exists() { parent } else { "./".into() };

                            self.open_explorer(parent, Some(path));
                            return false;
                        }
                    }

                    true
                })
            });
        }

        if input_event_len != ctx.input(|i| i.events.len()) {
            ctx.request_repaint();
            ctx.memory_mut(|memory| memory.stop_text_input());
        }

        self.debug_ui.show_window(ctx);
    }
}
