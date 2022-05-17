use crate::{
    image::{TextureView, TextureViewState},
    scroll::ScrollState,
};

#[derive(Default)]
pub struct VerticalReaderState {
    pub scroll_state: ScrollState,
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
            let scroll = crate::scroll::ScrollArea::both(state.scroll_state)
                .id_source(source_id)
                .show(ui, |ui| {
                    ui.centered_and_justified(|ui| {
                        ui.with_layout(egui::Layout::top_down(egui::Align::Center), |ui| {
                            for it in images.iter_mut() {
                                let available_size = ui.available_size();

                                TextureView::new(it).show(ui, |_, handle| {
                                    let size = handle.size_vec2();

                                    let max = size.x.min(available_size.x);

                                    crate::image::SplittedTextureWidget::new_with_scale(
                                        handle,
                                        max / size.x,
                                    )
                                });
                            }
                        })
                    })
                });

            state.scroll_state = scroll.state;

            // scroll
            //     .inner
            //     .response
            //     .widget_info(|| egui::WidgetInfo::new(egui::WidgetType::Other));

            scroll.inner.response
        })
        .inner
    }
}
