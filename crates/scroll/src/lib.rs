#![allow(clippy::all)]
use std::{hash::Hash, ops::RangeInclusive};

#[derive(Clone, Debug)]
pub struct ScrollState {
    pub offset: egui::Vec2,             // offset of the scroll
    pub size: Option<egui::Vec2>,       // size of the scrollable area
    pub inner_rect: Option<egui::Rect>, // size of viewable area
    pub is_stable: bool,
    pub vel: egui::Vec2,
    pub scroll_per_arrow: f32,
    pub scroll_per_page: f32,
    pub scroll_stuck_to_end: [bool; 2],
}

impl Default for ScrollState {
    fn default() -> Self {
        Self {
            offset: Default::default(),
            size: None,
            inner_rect: None,
            is_stable: false,
            scroll_per_arrow: 300.0,
            scroll_per_page: 900.0,
            vel: [0.0, 0.0].into(),
            scroll_stuck_to_end: [false, false],
        }
    }
}

impl ScrollState {
    pub fn reset(&mut self) {
        self.inner_rect.take();
        self.size.take();
        self.offset = egui::Vec2::ZERO;
        self.is_stable = false;
        self.vel = egui::Vec2::ZERO;
    }

    pub fn clamp_scroll(&mut self) -> ClampResult {
        if let Some(scroll_limit) = self.scroll_limit() {
            let scroll_offset = self.offset.clamp(egui::Vec2::ZERO, scroll_limit);

            if scroll_offset != self.offset {
                self.offset = scroll_offset;
                return ClampResult { clamped: true };
            }
        }

        ClampResult { clamped: false }
    }

    pub fn clamped_scroll(&self) -> Option<egui::Vec2> {
        self.scroll_limit()
            .map(|it| self.offset.clamp(egui::Vec2::ZERO, it))
    }

    pub fn scroll_by(&mut self, scroll_by: egui::Vec2) -> ScrollByResult {
        self.clamp_scroll();
        let scroll_offset = self.offset;
        self.offset += scroll_by;
        let clamped = self.clamp_scroll().clamped;

        let equal = |a: f32, b: f32| a == b || ((a - b).abs() <= 1.0);

        let changed_x = !equal(self.offset.x, scroll_offset.x);
        let changed_y = !equal(self.offset.y, scroll_offset.y);
        let changed = changed_x || changed_y;
        let changed_by = scroll_offset - self.offset;

        ScrollByResult {
            clamped,
            changed,
            changed_by,
        }
    }

    pub fn scroll_to_rect(&mut self, rect: egui::Rect, align: Option<egui::Align>) {
        for d in 0..2 {
            let range = rect.min[d]..=rect.max[d];

            self.scroll_to(d, range, align);
        }
    }

    pub fn scroll_to_response(&mut self, response: &egui::Response, align: Option<egui::Align>) {
        self.scroll_to_rect(response.rect, align)
    }

    pub fn min_rect(&self) -> egui::Rect {
        let inner_rect = self
            .inner_rect
            .unwrap_or(egui::Rect::from_min_max(egui::Pos2::ZERO, egui::Pos2::ZERO));
        let size = self.size.unwrap_or(egui::Vec2::ZERO);
        let offset = self.clamped_scroll().unwrap_or(egui::Vec2::ZERO);

        // egui::Rect::from_min_size(egui::pos2(0.0, 0.0), inner_rect.size())
        // inner_rect
        egui::Rect::from_min_size(inner_rect.min - offset, size)
    }

    pub fn clip_rect(&self) -> egui::Rect {
        let inner_rect = self
            .inner_rect
            .unwrap_or(egui::Rect::from_min_max(egui::Pos2::ZERO, egui::Pos2::ZERO));

        inner_rect
        // egui::Rect::from_min_size(self.offset.to_pos2(), inner_rect.size())
        // egui::Rect::from_min_size(inner_rect.min + self.offset, inner_rect.size())
    }

    fn scroll_to(&mut self, d: usize, range: RangeInclusive<f32>, align: Option<egui::Align>) {
        let min_rect = self.min_rect();
        let min = min_rect.min[d];
        let clip_rect = self.clip_rect();
        let visible_range = min..=min + clip_rect.size()[d];
        let start = *range.start();
        let end = *range.end();
        let clip_start = clip_rect.min[d];
        let clip_end = clip_rect.max[d];
        let mut spacing = 8f32;

        let delta = if let Some(align) = align {
            let center_factor = align.to_factor();

            let offset =
                egui::lerp(range, center_factor) - egui::lerp(visible_range, center_factor);

            // Depending on the alignment we need to add or subtract the spacing
            spacing *= egui::remap(center_factor, 0.0..=1.0, -1.0..=1.0);

            offset + spacing - self.offset[d]
        } else if start < clip_start && end < clip_end {
            -(clip_start - start + spacing).min(clip_end - end - spacing)
        } else if end > clip_end && start > clip_start {
            (end - clip_end + spacing).min(start - clip_start - spacing)
        } else {
            // Ui is already in view, no need to adjust scroll.
            0.0
        };

        self.offset[d] += delta;
    }

    pub fn scroll_limit(&self) -> Option<egui::Vec2> {
        self.size
            .map(|size| {
                if !size.any_nan() {
                    self.inner_rect
                        .map(|inner_rect| (size - inner_rect.size()).clamp(egui::Vec2::ZERO, size))
                } else {
                    None
                }
            })
            .flatten()
    }

    /// handle key (arrow, page, home or end)
    /// return true if event is handled
    pub fn handle_key_event(&mut self, event: &egui::Event) -> bool {
        self.handle_key_event_result(event)
            .map(|it| it.changed)
            .unwrap_or(false)
    }

    pub fn handle_key_event_result(&mut self, event: &egui::Event) -> Option<ScrollByResult> {
        let scroll_per_arrow = self.scroll_per_arrow;
        let scroll_per_page = self.scroll_per_page;
        let mut scroll_by = |direction: i8, reverse: bool, orientation: usize, step: f32| {
            let multiplier = if reverse { -1f32 } else { 1f32 };

            let direction = direction as f32 * multiplier;

            let mut scroll = egui::Vec2::ZERO;

            scroll[orientation] = step * direction;

            self.scroll_by(scroll)
        };

        macro_rules! handle {
            ($up:expr, $down:expr, $reverse:expr, $orientation:expr, $step:expr) => {{
                let handled = fmr_egui::key::handle_key_to_option(
                    true,
                    event,
                    $up,
                    $down,
                    |it| it.is_none(),
                    |direction| scroll_by(direction, $reverse, $orientation, $step),
                );

                if let Some(handled) = handled {
                    if handled.changed {
                        return Some(handled);
                    }
                }
            }};
        }

        handle!(
            egui::Key::ArrowUp,
            egui::Key::ArrowDown,
            false,
            1,
            scroll_per_arrow
        );
        handle!(
            egui::Key::PageUp,
            egui::Key::PageDown,
            false,
            1,
            scroll_per_page
        );
        handle!(egui::Key::Home, egui::Key::End, false, 1, f32::MAX);

        handle!(
            egui::Key::ArrowLeft,
            egui::Key::ArrowRight,
            false,
            0,
            scroll_per_arrow
        );
        {
            let handled = fmr_egui::key::handle_key_to_option(
                true,
                event,
                egui::Key::Home,
                egui::Key::End,
                |it| it.shift_only(),
                |direction| scroll_by(direction, false, 0, f32::MAX),
            );

            if let Some(handled) = handled {
                if handled.changed {
                    return Some(handled);
                }
            }
        }

        None
    }
}

#[derive(Debug)]
pub struct ScrollByResult {
    pub clamped: bool,
    pub changed: bool,
    pub changed_by: egui::Vec2,
}

pub struct ClampResult {
    clamped: bool,
}

pub struct ScrollOutput<R> {
    pub inner: R,
    pub state: ScrollState,
}

pub struct ScrollArea {
    id: egui::Id,
    state: ScrollState,
    has_bar: [bool; 2],
    scroll_per_wheel: egui::Vec2,
    scrolling_enabled: bool,
}

impl ScrollArea {
    pub fn new(state: ScrollState, has_bar: [bool; 2]) -> Self {
        Self {
            id: egui::Id::new("scroll-area"),
            state,
            has_bar,
            scroll_per_wheel: egui::vec2(300.0, 300.0),
            scrolling_enabled: true,
        }
    }

    pub fn vertical(state: ScrollState) -> Self {
        Self::new(state, [false, true])
    }

    pub fn both(state: ScrollState) -> Self {
        Self::new(state, [true, true])
    }

    pub fn id_source(mut self, id: impl Hash) -> Self {
        self.id = egui::Id::new(id);
        self
    }

    pub fn scroll_per_wheel(mut self, scroll: egui::Vec2) -> Self {
        self.scroll_per_wheel = scroll;
        self
    }

    pub fn enable_scrolling(mut self, enable: bool) -> Self {
        self.scrolling_enabled = enable;
        self
    }

    pub fn show<F, R>(self, ui: &mut egui::Ui, add_content: F) -> ScrollOutput<R>
    where
        F: FnOnce(&mut egui::Ui, &mut ScrollState) -> R,
    {
        let Self {
            id,
            mut state,
            has_bar,
            scroll_per_wheel,
            scrolling_enabled,
        } = self;
        state.clamp_scroll();

        let scroll_offset = state.clamped_scroll().unwrap_or(egui::Vec2::ZERO);
        let output = egui::ScrollArea::new(has_bar)
            .scroll_offset(scroll_offset)
            .id_source(id)
            .enable_scrolling(true)
            .show(ui, |ui| {
                let output = ui.allocate_ui(ui.available_size(), |ui| add_content(ui, &mut state));

                output
            });

        state.is_stable = state.inner_rect == Some(output.inner_rect)
            // prevent scrolled from egui when it isn't stable yet
            && (state.is_stable || output.state.offset == state.offset);
        state.inner_rect = Some(output.inner_rect);
        state.size = Some(output.inner.response.rect.size());

        let sense = if false {
            egui::Sense::drag()
        } else {
            egui::Sense::hover()
        };

        let response = output.inner.response.interact(sense);

        let offset_before = state.offset;
        if scrolling_enabled {
            if response.contains_pointer() && !ui.input(|input| input.pointer.any_down()) {
                let size = ui.input(|input| input.events.len());
                fmr_egui::event::handles(ui.ctx(), |event| {
                    if let egui::Event::MouseWheel {
                        unit: _,
                        delta,
                        modifiers,
                    } = event
                    {
                        if modifiers.ctrl || modifiers.command {
                            return false;
                        }
                        let mut scroll_by = [0.0; 2];

                        for i in 0..2 {
                            let step = delta[i] * -1.0;
                            let decrease_by = step.abs() * 50.0;
                            scroll_by[i] = step * scroll_per_wheel[i] - decrease_by;
                        }

                        return state
                            .scroll_by(egui::vec2(scroll_by[0], scroll_by[1]))
                            .changed;
                    }

                    false
                });

                if size != ui.input(|input| input.events.len()) {
                    ui.ctx().request_repaint();
                }
            }

            if !state.is_stable && response.dragged() {
                for d in 0..2 {
                    if has_bar[d] {
                        ui.input(|input| {
                            state.offset[d] -= input.pointer.delta()[d];
                            state.vel[d] = input.pointer.velocity()[d];
                            state.scroll_stuck_to_end[d] = false;
                        });
                    } else {
                        state.vel[d] = 0.0;
                    }
                }
            } else {
                let stop_speed = 20.0; // Pixels per second.
                let friction_coeff = 1000.0; // Pixels per second squared.
                let dt = ui.input(|input| input.unstable_dt);

                let friction = friction_coeff * dt;
                if friction > state.vel.length() || state.vel.length() < stop_speed {
                    state.vel = egui::Vec2::ZERO;
                } else {
                    state.vel -= friction * state.vel.normalized();
                    // Offset has an inverted coordinate system compared to
                    // the velocity, so we subtract it instead of adding it
                    state.offset -= state.vel * dt;
                    ui.ctx().request_repaint();
                }
            }
        }

        if state.is_stable && state.offset == offset_before {
            state.offset = output.state.offset;
        }

        if state.is_stable {
            if state.clamp_scroll().clamped {
                ui.ctx().request_repaint();
            }
        } else {
            ui.ctx().request_repaint();
        }

        ScrollOutput {
            inner: output.inner.inner,
            state,
        }

        // output.inner.inner
    }
}
