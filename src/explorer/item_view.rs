use crate::image::{SplittedTextureWidget, TextureView};

use super::ExplorerItem;

pub struct ExplorerItemView<'a, Item: ExplorerItem> {
    item: &'a mut Item,
    image_size: egui::Vec2,
    image_container_size: egui::Vec2,
    container_size: egui::Vec2,
    selected: bool,
    current_index: bool,
}

impl<'a, Item: ExplorerItem> ExplorerItemView<'a, Item> {
    pub fn new(item: &'a mut Item, image_container_size: egui::Vec2) -> Self {
        Self {
            item,
            image_container_size,
            image_size: image_container_size,
            container_size: image_container_size,
            selected: false,
            current_index: false,
        }
    }

    /// size of the item.
    pub fn container_size(mut self, size: egui::Vec2) -> Self {
        self.container_size = size;
        self
    }

    /// is this item is selected.
    pub fn selected(mut self, selected: bool) -> Self {
        self.selected = selected;
        self
    }

    /// is this item is the current index.
    /// this is different from selected, selected can be more than one,
    /// this should be only one.
    pub fn current_index(mut self, current_index: bool) -> Self {
        self.current_index = current_index;
        self
    }

    /// size of the image
    pub fn image_size(mut self, size: egui::Vec2) -> Self {
        self.image_size = size;
        self
    }

    pub fn show(self, ui: &mut egui::Ui) -> egui::Response {
        let Self {
            item,
            image_size,
            selected,
            current_index,
            container_size,
            image_container_size,
        } = self;

        let response = ui
            .vertical_centered(|ui| {
                ui.allocate_ui(container_size, |ui| {
                    ui.allocate_ui(image_container_size, |ui| {
                        ui.centered_and_justified(|ui| {
                            TextureView::new(item.thumbnail()).show(ui, |_, handle| {
                                let scale = image_size / handle.size_vec2();
                                SplittedTextureWidget::new_with_scale(handle, scale.min_elem())
                            });
                        })
                    });

                    ui.allocate_ui(egui::vec2(ui.available_width() * 0.8, 0.0), |ui| {
                        ui.centered_and_justified(|ui| {
                            ui.add(egui::Label::new(item.name()).wrap(true));

                            ui.add_space(10.0);
                        });
                    });

                    ui.add_space(ui.available_height());
                });
            })
            .response;

        let response = response.interact(egui::Sense::hover());
        let painter = ui.painter();

        let stroke = if selected {
            let mut stroke = ui.visuals().selection.stroke;
            // let mut stroke = egui::Stroke::new(10.0, egui::Color32::BLUE);
            if current_index {
                stroke.width *= 3.0;
            }
            stroke
        } else {
            if response.hovered() {
                ui.visuals().widgets.hovered.fg_stroke
            } else {
                ui.visuals().widgets.inactive.fg_stroke
            }
        };

        let response = response.interact(egui::Sense::click());
        let mut visuals = ui.style().interact_selectable(&response, selected);

        if selected {
            visuals.fg_stroke.width *= 1.5;

            if current_index {
                visuals.fg_stroke.width *= 2.0;
            }
        }

        let rect = egui::Rect::from_min_max(
            response.rect.min + egui::vec2(1.0, 1.0),
            response.rect.max - egui::vec2(stroke.width, stroke.width),
        );

        painter.rect_stroke(
            rect.expand(visuals.expansion),
            visuals.rounding,
            visuals.fg_stroke,
        );

        response
    }
}
