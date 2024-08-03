mod app;
mod app_explorer;
mod app_reader;
// pub mod image_search;
pub mod inspection;
mod storage;
mod wake;
use std::{collections::BTreeMap, sync::Arc};

pub use app::*;
pub use app_explorer::*;
pub use app_reader::*;

use eframe::egui;
use parking_lot::Mutex;

pub trait UnsignedExt {
    fn checked_add_signed_ext(self, rhs: isize) -> Option<usize>;
}

impl UnsignedExt for usize {
    fn checked_add_signed_ext(self, rhs: isize) -> Option<usize> {
        if rhs < 0 {
            self.checked_sub(rhs.wrapping_abs() as usize)
        } else {
            self.checked_add(rhs as usize)
        }
    }
}

pub fn setup_custom_fonts(ctx: &egui::Context) {
    let mut fonts = egui::FontDefinitions::default();

    let mut load = |name: &str, mut font: egui::FontData| {
        let name = name.to_string();
        font.tweak.scale = 1.3;
        fonts.font_data.insert(name.clone(), font);

        fonts
            .families
            .entry(egui::FontFamily::Proportional)
            .or_default()
            .push(name.clone());

        fonts
            .families
            .entry(egui::FontFamily::Monospace)
            .or_default()
            .push(name);
    };

    macro_rules! load_adobe {
        ($($prefix:literal),+) => {
            $(
                let bytes = include_bytes!(
                    concat!(
                        "../fonts/adobe-source-han-sans/SourceHanSans",
                        $prefix,
                        "-Normal.otf"
                    )
                );
                load(
                    concat!("SourceHanSans", $prefix, "-Normal"),
                    egui::FontData::from_static(bytes)
                );
            )+
        };
    }
    load_adobe!("JP", "CN", "TW", "KR");

    ctx.set_fonts(fonts);
}

#[derive(Debug, Default, Clone, serde::Serialize, serde::Deserialize)]
pub struct ReadingProgress {
    pub progress: Arc<Mutex<BTreeMap<String, ReadingProgressValue>>>,
}

#[derive(Debug, Clone, serde::Serialize, serde::Deserialize)]
pub struct ReadingProgressValue {
    pub page: usize,
    pub total_page: usize,
    pub time: std::time::SystemTime,

    #[serde(default)]
    pub is_finished: bool,
}

impl Default for ReadingProgressValue {
    fn default() -> Self {
        Self {
            page: 0,
            total_page: 0,
            time: std::time::SystemTime::now(),
            is_finished: false,
        }
    }
}

impl ReadingProgress {
    pub fn insert(&self, path: &std::path::Path, mut value: ReadingProgressValue) {
        if let Some(it) = path.as_os_str().to_str() {
            match self.progress.lock().entry(it.to_string()) {
                std::collections::btree_map::Entry::Vacant(entry) => {
                    entry.insert(value);
                }
                std::collections::btree_map::Entry::Occupied(mut entry) => {
                    let entry = entry.get_mut();

                    if value.page < entry.page {
                        value.page = entry.page;
                    }

                    value.is_finished = entry.is_finished || value.is_finished;

                    *entry = value;
                }
            }
        }
    }

    pub fn insert_finish(&self, path: &std::path::Path) {
        if let Some(it) = path.as_os_str().to_str() {
            match self.progress.lock().entry(it.to_string()) {
                std::collections::btree_map::Entry::Vacant(_) => {}
                std::collections::btree_map::Entry::Occupied(mut item) => {
                    item.get_mut().page = item.get().total_page;
                    item.get_mut().is_finished = true;
                }
            }
        }
    }
}

impl ReadingProgressValue {
    pub fn new(page: usize, total_page: usize) -> Self {
        Self {
            page,
            total_page,
            time: std::time::SystemTime::now(),
            is_finished: false,
        }
    }
}
