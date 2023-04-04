pub fn handle_key(
    enable: bool,
    event: &egui::Event,
    up: egui::Key,
    down: egui::Key,
    modifiers_p: impl FnOnce(&egui::Modifiers) -> bool,
    action: impl FnOnce(i8) -> bool,
) -> bool {
    handle_key_to_option(enable, event, up, down, modifiers_p, action).unwrap_or(false)
}

pub fn handle_key_to_option<F, R>(
    enable: bool,
    event: &egui::Event,
    up: egui::Key,
    down: egui::Key,
    modifiers_p: impl FnOnce(&egui::Modifiers) -> bool,
    action: F,
) -> Option<R>
where
    F: FnOnce(i8) -> R,
{
    if enable {
        if let egui::Event::Key {
            key,
            modifiers,
            pressed: true,
            repeat: _,
        } = event
        {
            let step = if *key == up {
                -1
            } else if *key == down {
                1
            } else {
                return None;
            };

            if modifiers_p(modifiers) {
                return Some(action(step));
            }
        }
    }

    None
}
