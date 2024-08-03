use fmr_frame::{SplittedTextureWidget, TextureView, TextureViewState};

use super::Sizer;
use fmr_scroll::{ScrollArea, ScrollState};

#[derive(Default)]
pub struct VerticalReaderState {
    pub scroll_state: ScrollState,

    pub sizer: Sizer,
}

impl VerticalReaderState {
    pub fn handle_event(&mut self, event: &egui::Event) -> bool {
        self.scroll_state.handle_key_event(event)
    }
}

pub struct VerticalReader<'a> {
    images: &'a mut Vec<TextureViewState>,
    state: &'a mut VerticalReaderState,
    source_id: Option<egui::Id>,
}

impl<'a> VerticalReader<'a> {
    pub fn new(images: &'a mut Vec<TextureViewState>, state: &'a mut VerticalReaderState) -> Self {
        Self {
            images,
            state,
            source_id: None,
        }
    }

    pub fn id_source(mut self, id_source: impl std::hash::Hash) -> Self {
        self.source_id = Some(egui::Id::new(id_source));
        self
    }

    pub fn show(self, ui: &mut egui::Ui) -> egui::Response {
        let VerticalReader {
            images,
            source_id,
            state,
        } = self;
        ui.scope(|ui| {
            let scroll = ScrollArea::both(state.scroll_state.clone())
                .id_source(source_id)
                .show(ui, |ui, _| {
                    ui.centered_and_justified(|ui| {
                        ui.with_layout(egui::Layout::top_down(egui::Align::Center), |ui| {
                            for it in images.iter_mut() {
                                let image_max_size = it.max_size().map(|it| it as f32);
                                let available_size = ui.available_size() - egui::vec2(1.0, 1.0);
                                let scale = state.sizer.calc_range(
                                    image_max_size.into(),
                                    available_size,
                                    [true, false],
                                );

                                TextureView::new(it).show(ui, |_, handle| {
                                    SplittedTextureWidget::new_with_scale(handle, scale)
                                });
                            }
                        })
                    })
                });

            state.scroll_state = scroll.state;

            scroll.inner.response
        })
        .inner
    }
}
