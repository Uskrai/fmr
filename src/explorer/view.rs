use std::hash::Hash;

use super::{Explorer, ExplorerItem, ExplorerItemView, ExplorerSetting};

pub struct ExplorerView<'a, Item: ExplorerItem> {
    explorer: &'a mut Explorer<Item>,
    setting: &'a ExplorerSetting,
    id: egui::Id,
}

pub struct ExplorerOutput {
    /// is selected content double clicked.
    pub is_double_clicked: bool,

    pub column: usize,

    pub grid_response: egui::Response,

    responses: Vec<egui::Response>,
}

impl<'a, Item: ExplorerItem> ExplorerView<'a, Item> {
    pub fn new(explorer: &'a mut Explorer<Item>, setting: &'a ExplorerSetting) -> Self {
        Self {
            explorer,
            setting,
            id: egui::Id::new("explorer"),
        }
    }

    pub fn id_source(mut self, id: impl Hash) -> Self {
        self.id = egui::Id::new(id);
        self
    }

    pub fn show(mut self, ui: &mut egui::Ui) -> ExplorerOutput {
        let scroll = crate::scroll::ScrollArea::vertical(self.explorer.scroll.clone())
            .id_source(self.id.with("scroll-area"))
            // .enable_scrolling(true)
            .show(ui, |ui, _| self.show_grid(ui));

        self.explorer.scroll = scroll.state;
        if self.explorer.scroll_to_index {
            if let Some(response) = scroll.inner.responses.get(self.explorer.current) {
                // self.explorer.scroll
                // println!("scroll to me");
                // response.scroll_to_me(None);
                self.explorer.scroll.scroll_to_response(response, None);
                ui.ctx().request_repaint();
                self.explorer.scroll_to_index = false;
            }
        }

        crate::egui_event::handles_ui(ui, |ui, event| {
            Self::handle_key(self.explorer, &scroll.inner, ui, event)
        });
        crate::egui_event::handles_ui(ui, |ui, event| {
            let handled = self.explorer.scroll.handle_key_event(event);
            if handled {
                ui.ctx().request_repaint();
            }
            handled
        });

        scroll.inner
    }

    fn show_grid(&mut self, ui: &mut egui::Ui) -> ExplorerOutput {
        let Self {
            explorer,
            setting,
            id,
        } = self;

        let available_width = ui.available_width();

        let column = (available_width / (setting.minimum_column_width as f32))
            .floor()
            .max(1.0) as usize;
        let width = (available_width / (column as f32)).floor() - (ui.spacing().item_spacing.x);

        if explorer.column_count != column {
            explorer.row_size.clear();
        }
        explorer.column_count = column;

        let mut is_double_clicked = false;
        let mut responses = vec![];

        let grid = egui::Grid::new(id.with("grid"))
            .max_col_width(width)
            .show(ui, |ui| {
                let mut row = 0;
                for (i, it) in explorer.content.iter_mut().enumerate() {
                    ui.push_id(i, |ui| {
                        let width = ui.available_width();
                        let size = explorer
                            .row_size
                            .entry(row)
                            .or_insert_with(|| egui::vec2(width, width));

                        let response = ExplorerItemView::new(it, egui::vec2(width, width))
                            .container_size(*size)
                            .image_size(egui::vec2(width * 0.8, width))
                            .selected(i == explorer.current)
                            .current_index(i == explorer.current)
                            .show(ui);

                        let response = response.interact(egui::Sense::click());

                        let max_size = response.rect.size().max(*size);

                        if max_size != *size {
                            ui.ctx().request_repaint();
                            *size = max_size;
                        }
                        *size = response.rect.size().max(*size);

                        if response.clicked() {
                            explorer.current = i;
                            ui.ctx().request_repaint();
                        }

                        if response.double_clicked() {
                            is_double_clicked = true;
                        }

                        // if is_double_clicked || response.clicked_by(egui::PointerButton::Extra2) {
                        //     is_double_clicked = true;
                        // }

                        responses.push(response);
                    });

                    let column = column as isize;
                    if ((i as isize) % column) == (column - 1) {
                        row += 1;
                        ui.end_row();
                    }
                }
            });

        ExplorerOutput {
            is_double_clicked,
            column,
            grid_response: grid.response,
            responses,
        }
    }

    // return true if event is handled
    pub fn handle_key(
        explorer: &mut Explorer<Item>,
        output: &ExplorerOutput,
        ui: &mut egui::Ui,
        event: &egui::Event,
    ) -> bool {
        let mut handled = false;

        let mut move_by = |step: isize| {
            let changed = explorer.move_by(step);

            if changed {
                if let Some(response) = output.responses.get(explorer.current) {
                    explorer.scroll.scroll_to_response(response, None);
                    // println!("scroll to me");
                    // response.scroll_to_me(None);
                    ui.ctx().request_repaint();
                }
            }

            changed
        };

        handled |= crate::key::handle_key(
            !handled,
            event,
            egui::Key::ArrowUp,
            egui::Key::ArrowDown,
            |it| it.is_none(),
            |step| {
                let step = (step as isize) * (output.column as isize);
                move_by(step)
            },
        );

        handled |= crate::key::handle_key(
            !handled,
            event,
            egui::Key::ArrowLeft,
            egui::Key::ArrowRight,
            |it| it.is_none(),
            |step| move_by(step as isize),
        );

        handled
    }
}
