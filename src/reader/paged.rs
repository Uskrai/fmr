use crate::{
    image::{TextureView, TextureViewState},
    key::handle_key,
    UnsignedExt, Vec2Ext,
};

use super::Sizer;

pub struct PagedReader<'a> {
    images: &'a mut Vec<TextureViewState>,
    state: &'a mut PagedReaderState,
}

#[derive(Default)]
pub struct PagedReaderState {
    pub scroll: crate::scroll::ScrollState,

    pub sizer: Sizer,

    pub index: usize,

    pub id: String,

    pub read_from_right: bool,

    pub index_text: HideTextState,
}

pub struct HideTextState {
    waker: crate::wake::EguiWaker,
    duration: std::time::Duration,
    pub last_reset: std::time::Instant,
    //
    // pub duration: std::time::Duration,
}

impl Default for HideTextState {
    fn default() -> Self {
        Self {
            waker: Default::default(),
            last_reset: std::time::Instant::now(),
            duration: std::time::Duration::from_secs(1),
        }
    }
}

impl HideTextState {
    pub fn reset(&mut self) {
        self.last_reset = std::time::Instant::now();
    }

    pub fn should_show(&self) -> bool {
        let elapsed = self.last_reset.elapsed();

        elapsed < self.duration
    }
}

impl PagedReaderState {
    // return true if changed
    pub fn change_index_by(&mut self, step: isize, limit: usize) -> bool {
        let index = self.index.checked_add_signed_ext(step);

        if let Some(index) = index {
            if index < limit {
                let changed = self.index != index;
                if changed {
                    self.reset(step);
                    self.index = index;
                    return true;
                }
            }
        }
        false
    }

    pub fn reset(&mut self, step: isize) {
        self.scroll.reset();
        self.index_text.reset();
        if step.is_positive() && self.read_from_right {
            self.scroll.offset.x = f32::MAX;
        } else if step.is_negative() {
            self.scroll.offset = egui::vec2(f32::MAX, f32::MAX);

            if self.read_from_right {
                self.scroll.offset.x = 0.0;
            }
        }
    }

    pub fn handle_event(&mut self, images: &Vec<TextureViewState>, event: &egui::Event) -> bool {
        let mut handled = false;
        let mut first = true;

        if self.scroll.handle_key_event(event) {
            return true;
        }

        let read_from_right = self.read_from_right;

        let mut change_page_with_key =
            |up: egui::Key, down: egui::Key, reverse: bool, use_ctrl: bool| {
                let current = handle_key(
                    first,
                    event,
                    up,
                    down,
                    |it| it.is_none() || (it.command_only() && use_ctrl),
                    |it| {
                        let multiplier = if reverse { -1 } else { 1 };

                        let step = it as isize * multiplier;

                        self.change_index_by(step, images.len())
                    },
                );
                first = first && !current;
                handled |= current;
            };

        change_page_with_key(egui::Key::ArrowUp, egui::Key::ArrowDown, false, true);
        change_page_with_key(
            egui::Key::ArrowLeft,
            egui::Key::ArrowRight,
            read_from_right,
            true,
        );

        handled
    }
}

impl<'a> PagedReader<'a> {
    pub fn new(images: &'a mut Vec<TextureViewState>, state: &'a mut PagedReaderState) -> Self {
        Self { images, state }
    }

    pub fn change_index_by(&mut self, step: isize) -> bool {
        self.state.change_index_by(step, self.images.len())
    }

    pub fn show(mut self, ui: &mut egui::Ui) -> egui::Response {
        let Self { images, state } = &mut self;

        let response = if state.index < images.len() {
            let image = &mut images[state.index];

            let output = crate::scroll::ScrollArea::both(state.scroll.clone())
                // .scroll_bar_visibility(scroll_bar_visibility)
                .show(ui, |ui, _| {
                    let scroll_bar_width = ui.spacing().scroll.bar_inner_margin
                        + ui.spacing().scroll.bar_width
                        + ui.spacing().scroll.bar_outer_margin;

                    // let image_max_size = [1200.0, 638.0];
                    let image_max_size = image.max_size().map(|it| it as f32);
                    // let available_size =
                    //     ui.available_size() - egui::vec2(1.0,1.0);
                    let available_size =
                        ui.available_size() - egui::vec2(scroll_bar_width, scroll_bar_width);
                    let scale = state.sizer.calc(image_max_size.into(), available_size);
                    let image_size = egui::Vec2::from(image_max_size) * scale;

                    let show_image = |ui: &mut egui::Ui| {
                        TextureView::new(image).show(ui, |_, handle| {
                            crate::image::SplittedTextureWidget::new_with_scale(handle, scale)
                        })
                    };

                    if image_size.x <= available_size.x {
                        ui.centered_and_justified(show_image).inner
                    } else {
                        ui.vertical(show_image).inner
                    }
                });

            state.scroll = output.state;

            if image.is_loading() {
                state.reset(1);
            }

            if state.index_text.should_show() {
                self.draw_index(ui);
            }

            output.inner
        } else {
            ui.centered_and_justified(|ui| ui.label("Loading")).inner
        };

        // TODO: change this to response.has_focus()
        if response.contains_pointer() {
            let size = ui.input(|input| input.events.len());
            let any_down = ui.input(|it| it.pointer.any_down());
            let primary_down = ui.input(|it| it.pointer.primary_down());
            let secondary_down = ui.input(|it| it.pointer.secondary_down());

            let should_change_page =
                !primary_down && (!any_down || secondary_down) && response.contains_pointer();

            crate::egui_event::handles(ui.ctx(), |event| {
                if should_change_page {
                    if let egui::Event::Scroll(scroll) = event {
                        let mut step = scroll.to_step();
                        step[0] *= if self.state.read_from_right { -1 } else { 1 };

                        let ret = match step {
                            [0, i] | [i, 0] => self.change_index_by(i),
                            _ => false,
                        };
                        return ret;
                    }
                }

                false
            });

            if ui.input(|input| input.events.len()) != size {
                ui.ctx().request_repaint();
            }
        }

        if response.clicked() {
            self.change_index_by(1);
        } else if response.secondary_clicked() {
            self.change_index_by(-1);
        }

        response
    }

    fn draw_index(&mut self, ui: &mut egui::Ui) {
        let Self { state, images } = self;

        let text = format!("{}/{}", state.index + 1, images.len());
        let text = egui::RichText::new(text)
            .background_color(egui::Color32::WHITE)
            .strong()
            .monospace()
            .color(egui::Color32::BLACK);

        let galley = egui::WidgetText::RichText(text).into_galley(
            ui,
            None,
            ui.available_width(),
            egui::FontSelection::FontId(egui::FontId::proportional(24.0)),
        );

        let rect = state.scroll.inner_rect;
        if let Some(rect) = rect {
            let rect = egui::Rect::from_min_max(
                rect.left_bottom() - egui::vec2(0.0, galley.size().y + 5.0),
                rect.max,
            );
            let pos = rect.center() - (galley.size() / 2.0);
            ui.painter().galley(pos, galley, egui::Color32::TRANSPARENT);

            let ctx = ui.ctx().clone();
            state
                .index_text
                .waker
                .call(state.index_text.duration, || async move {
                    ctx.request_repaint()
                });
        }
    }
}
