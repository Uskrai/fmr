use serde::{Deserialize, Serialize};

#[derive(Clone, PartialEq, Eq, Serialize, Deserialize, Default, Debug)]
pub struct Sizer {
    pub shrink: bool,
    pub enlarge: bool,
    fit: [bool; 2],
}

impl Sizer {
    pub fn set_fit_horizontal(&mut self, fit: bool) {
        self.fit[0] = fit;
    }

    pub fn set_fit_vertical(&mut self, fit: bool) {
        self.fit[1] = fit;
    }

    pub fn fit_horizontal_mut(&mut self) -> &mut bool {
        &mut self.fit[0]
    }

    pub fn fit_vertical_mut(&mut self) -> &mut bool {
        &mut self.fit[1]
    }

    pub fn calc(&self, size: egui::Vec2, fit_to: egui::Vec2) -> f32 {
        let mut scale = [None; 2];
        for i in 0..2 {
            scale[i] = if self.fit[i] {
                self.calc_side(size[i], fit_to[i])
            } else {
                None
            };
        }

        match scale {
            // fit one side
            [Some(scale), None] | [None, Some(scale)] => scale,
            // fit both side, prefer minimum.
            [Some(x), Some(y)] => x.min(y),
            // no fit
            [None, None] => 1.0,
        }
    }

    fn calc_side(&self, size: f32, fit_to: f32) -> Option<f32> {
        // let fit_to = fit_to  1.0;
        let cmp = size
            .partial_cmp(&fit_to)
            .unwrap_or(std::cmp::Ordering::Equal);

        let should_shrink = self.shrink && cmp.is_gt();
        let should_enlarge = self.enlarge && cmp.is_lt();

        if should_shrink || should_enlarge {
            Some((fit_to) / size)
        } else {
            None
        }
    }
}
