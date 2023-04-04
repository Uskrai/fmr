#[derive(Default)]
pub struct DebugUI {
    inspection_ui: bool,
    memory_ui: bool,
    style_ui: bool,
    setting_ui: bool,
    texture_ui: bool,
}

impl DebugUI {
    pub fn show_menu(&mut self, ui: &mut egui::Ui) {
        let mut open = |value: &mut bool, name: &str| {
            if ui.button(name).clicked() {
                *value = true;
                ui.close_menu();
            }
        };
        open(&mut self.style_ui, "Style UI");
        open(&mut self.inspection_ui, "Inspection UI");
        open(&mut self.memory_ui, "Memory UI");
        open(&mut self.texture_ui, "Texture UI");
        open(&mut self.setting_ui, "Setting UI");
    }

    pub fn show_window(&mut self, ctx: &egui::Context) {
        egui::Window::new("texture-ui")
            .open(&mut self.texture_ui)
            .show(ctx, |ui| ctx.texture_ui(ui));
        egui::Window::new("setting-ui")
            .open(&mut self.setting_ui)
            .show(ctx, |ui| ctx.settings_ui(ui));
        egui::Window::new("inspection-ui")
            .open(&mut self.inspection_ui)
            .show(ctx, |ui| ctx.inspection_ui(ui));
        egui::Window::new("memory-ui")
            .open(&mut self.memory_ui)
            .show(ctx, |ui| {
                ctx.memory_ui(ui);
                ui.label(format!("Focus: {:?}", ctx.memory(|memory| memory.focus())));
            });
        egui::Window::new("style-ui")
            .open(&mut self.style_ui)
            .show(ctx, |ui| ctx.style_ui(ui));
    }
}
