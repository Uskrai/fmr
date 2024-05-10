mod cache;
mod item_view;
mod loader;
mod view;

use std::{collections::HashMap, path::PathBuf};

pub use self::cache::*;
pub use self::item_view::*;
pub use self::loader::*;
pub use self::view::*;
use derivative::Derivative;
use serde::{Deserialize, Serialize};

use crate::{image::TextureViewState, scroll::ScrollState, UnsignedExt};

pub struct Explorer<Item: ExplorerItem> {
    pub scroll: ScrollState,
    pub content: Vec<Item>,
    current: usize,
    pub id: egui::Id,
    row_size: HashMap<usize, egui::Vec2>,
    pub column_count: usize,
    scroll_to_index: bool,
}

#[derive(Clone, Debug, PartialEq, Serialize, Deserialize, Derivative)]
#[derivative(Default)]
#[serde(default)]
pub struct ExplorerSetting {
    #[derivative(Default(value = "400"))]
    pub minimum_column_width: u32,
}

impl<Item: ExplorerItem> Default for Explorer<Item> {
    fn default() -> Self {
        Self {
            content: Default::default(),
            scroll: Default::default(),
            current: 0,
            id: egui::Id::new("explorer"),
            row_size: HashMap::new(),
            column_count: 0,
            scroll_to_index: false,
        }
    }
}

impl<Item: ExplorerItem> Explorer<Item> {
    pub fn move_by(&mut self, step: isize) -> bool {
        if let Some(it) = self.current.checked_add_signed_ext(step) {
            self.set_index(it)
        } else {
            false
        }
    }

    pub fn current_index(&self) -> usize {
        self.current
    }

    pub fn set_index(&mut self, index: usize) -> bool {
        if index < self.content.len() {
            self.current = index;
            true
        } else {
            false
        }
    }

    pub fn set_index_force(&mut self, index: usize) {
        self.current = index;
    }

    pub fn scroll_to_current(&mut self) {
        self.scroll_to_index = true;
    }

    pub fn items(&self) -> &Vec<Item> {
        &self.content
    }

    pub fn current_item(&self) -> Option<&Item> {
        self.content.get(self.current)
    }
}

pub trait ExplorerItem {
    fn name(&self) -> &str;
    fn thumbnail(&mut self) -> &mut TextureViewState;
}

pub struct PathExplorerItem {
    name: String,
    pub path: PathBuf,
    pub thumbnail: TextureViewState,
}

impl PathExplorerItem {
    /// Create new PathExplorerItem, None iff path.file_name return None.
    pub fn new(path: PathBuf) -> Option<Self> {
        Some(Self {
            name: path.file_name()?.to_string_lossy().to_string(),
            path,
            thumbnail: TextureViewState::Loading(Default::default()),
        })
    }
}

impl ExplorerItem for PathExplorerItem {
    fn name(&self) -> &str {
        &self.name
    }

    fn thumbnail(&mut self) -> &mut TextureViewState {
        &mut self.thumbnail
    }
}
