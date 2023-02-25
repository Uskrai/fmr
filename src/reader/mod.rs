pub mod loader;
mod paged;
mod sizer;
mod vertical;
pub use paged::*;
use serde::{Deserialize, Serialize};
pub use sizer::*;
pub use vertical::*;

use crate::image::TextureViewState;

#[derive(Clone, PartialEq, Eq, Deserialize, Serialize, Debug)]
#[serde(default)]
pub struct ReaderSetting {
    pub mode: ReaderMode,
    pub paged: ReaderPagedSetting,

    pub sizer: Sizer,
    pub scroll_per_arrow: i32,
    pub scroll_per_page: i32,
}

impl Default for ReaderSetting {
    fn default() -> Self {
        Self {
            mode: Default::default(),
            paged: Default::default(),
            sizer: Sizer::default(),
            scroll_per_page: 900,
            scroll_per_arrow: 300,
        }
    }
}

#[derive(Clone, PartialEq, Eq, Deserialize, Serialize, Default, Debug)]
#[serde(default)]
pub struct ReaderPagedSetting {
    pub read_from_right: bool,
}

#[derive(Clone, PartialEq, Eq, Deserialize, Serialize, Debug)]
pub enum ReaderMode {
    Vertical,
    Paged,
}

impl Default for ReaderMode {
    fn default() -> Self {
        Self::Paged
    }
}

pub struct Reader {
    pub images: Vec<TextureViewState>,
    pub state: ReaderModeState,
}

impl Reader {
    pub fn new(images: Vec<TextureViewState>, mode: ReaderModeState) -> Self {
        Self {
            images,
            state: mode,
        }
    }

    pub fn is_read_from_right(&self) -> bool {
        if let ReaderModeState::Paged(state) = &self.state {
            state.read_from_right
        } else {
            false
        }
    }

    pub fn handle_event(&mut self, event: &egui::Event) -> bool {
        match &mut self.state {
            ReaderModeState::Vertical(state) => state.handle_event(event),
            ReaderModeState::Paged(state) => state.handle_event(&self.images, event),
        }
    }
}

pub enum ReaderModeState {
    Vertical(VerticalReaderState),
    Paged(PagedReaderState),
}

impl ReaderModeState {
    pub fn paged() -> Self {
        Self::Paged(Default::default())
    }

    pub fn paged_with_index(index: usize) -> Self {
        Self::Paged(PagedReaderState {
            index,
            ..Default::default()
        })
    }

    /// Returns `true` if the manga reader mode is [`Vertical`].
    ///
    /// [`Vertical`]: ReaderMode::Vertical
    #[must_use]
    pub fn is_vertical(&self) -> bool {
        matches!(self, Self::Vertical(..))
    }

    /// Returns `true` if the manga reader mode is [`Paged`].
    ///
    /// [`Paged`]: ReaderMode::Paged
    #[must_use]
    pub fn is_paged(&self) -> bool {
        matches!(self, Self::Paged(..))
    }
}

pub struct ReaderView<'a> {
    pub reader: &'a mut Reader,
    pub setting: &'a ReaderSetting,
}

impl<'a> ReaderView<'a> {
    pub fn new(reader: &'a mut Reader, setting: &'a ReaderSetting) -> Self {
        Self { reader, setting }
    }

    pub fn show(self, ui: &mut egui::Ui) -> egui::Response {
        let Self { reader, setting } = self;
        match &mut reader.state {
            ReaderModeState::Vertical(state) => {
                state.sizer = setting.sizer.clone();
                state.scroll_state.scroll_per_page = setting.scroll_per_page as f32;
                state.scroll_state.scroll_per_arrow = setting.scroll_per_arrow as f32;
                VerticalReader::new(&mut reader.images, state).show(ui)
            }
            ReaderModeState::Paged(state) => {
                state.sizer = setting.sizer.clone();
                state.read_from_right = setting.paged.read_from_right;
                state.scroll.scroll_per_page = setting.scroll_per_page as f32;
                state.scroll.scroll_per_arrow = setting.scroll_per_arrow as f32;
                PagedReader::new(&mut reader.images, state).show(ui)
            }
        }
    }
}
