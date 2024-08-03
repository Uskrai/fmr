pub mod event;
pub mod key;
pub mod tools;

pub trait Vec2Ext {
    /// convert vector to step (-1 or 1) (-1 when negative, and 1 when positive)
    fn to_step(self) -> [isize; 2];
}

impl Vec2Ext for egui::Vec2 {
    fn to_step(self) -> [isize; 2] {
        let mut ret = [0; 2];
        for i in 0..2 {
            ret[i] = (self[i] / -self[i].abs()) as isize;
        }
        ret
    }
}


