use serde::{Deserialize, Serialize};

#[derive(Clone, PartialEq, Eq, Serialize, Deserialize, Debug)]
#[serde(default)]
pub struct Sizer {
    pub shrink: bool,
    pub enlarge: bool,
    pub fit_to_scale: u64,
    fit: [bool; 2],
}

impl Default for Sizer {
    fn default() -> Self {
        Self {
            shrink: false,
            enlarge: false,
            fit_to_scale: 100,
            fit: [false, false],
        }
    }
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
        self.calc_range(size, fit_to, [true, true])
    }

    pub fn calc_range(&self, size: egui::Vec2, fit_to: egui::Vec2, range: [bool; 2]) -> f32 {
        let mut scale = [None; 2];
        for (i, it) in range.iter().enumerate() {
            if *it {
                scale[i] = if self.fit[i] {
                    self.calc_side(size[i], fit_to[i])
                } else {
                    None
                }
            }
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
            let fit_to_scale = (self.fit_to_scale as f32) / 100f32;
            Some((fit_to * fit_to_scale) / size)
        } else {
            None
        }
    }
}
