use std::{
    sync::{
        atomic::{AtomicBool, Ordering},
        Arc,
    },
    time::Duration,
};

use eframe::epaint::mutex::RwLock as EguiRwLock;
use eframe::epaint::TextureManager;
use parking_lot::Mutex;

use crate::image::EguiSplittedFrameData;

use super::EguiSplittedImageData;

#[derive(Clone)]
pub struct SplittedTextureHandle {
    name: String,
    handles: Vec<Vec<egui::TextureHandle>>,
}

impl SplittedTextureHandle {
    pub fn new(name: String, handles: Vec<Vec<egui::TextureHandle>>) -> Self {
        Self { name, handles }
    }

    pub fn size(&self) -> [usize; 2] {
        let width;
        let mut height = 0;

        let mut first_width = 0;
        for it in &self.handles[0] {
            first_width += it.size()[0];
        }

        for vec in &self.handles {
            let curr_height = vec[0].size()[1];
            let mut curr_width = 0;
            for it in vec {
                let size = it.size();
                assert_eq!(
                    curr_height, size[1],
                    "All height should be equal to first height of current outer vector"
                );
                curr_width += size[0];
            }

            assert_eq!(
                first_width, curr_width,
                "Sum of all inner vector's width should be equal"
            );

            height += curr_height;
        }

        width = first_width;

        [width, height]
    }

    pub fn size_vec2(&self) -> egui::Vec2 {
        self.size().map(|it| it as f32).into()
    }
}

pub struct SplittedTextureWidget<'a> {
    texture: &'a SplittedTextureHandle,
    size: egui::Vec2,
    sense: egui::Sense,
}

impl<'a> SplittedTextureWidget<'a> {
    pub fn new(texture: &'a SplittedTextureHandle, size: impl Into<egui::Vec2>) -> Self {
        Self {
            texture,
            size: size.into(),
            sense: egui::Sense::hover(),
        }
    }

    pub fn new_with_scale(texture: &'a SplittedTextureHandle, scale: f32) -> Self {
        Self {
            texture,
            size: texture.size_vec2(),
            sense: egui::Sense::hover(),
        }
        .scale(scale)
    }

    pub fn size(mut self, size: impl Into<egui::Vec2>) -> Self {
        self.size = size.into();
        self
    }

    pub fn scale(mut self, scale: f32) -> Self {
        self.size = self.texture.size_vec2() * scale;
        self
    }

    pub fn sense(mut self, sense: egui::Sense) -> Self {
        self.sense = sense;
        self
    }

    pub fn paint_at(&self, ui: &mut egui::Ui, rect: egui::Rect) {
        let size = self.size;
        let scale = size / self.texture.size_vec2();

        let (mut x, mut y) = (0.0, 0.0);
        let mut size;

        for vertical in &self.texture.handles {
            size = egui::Vec2::ZERO;

            for horizontal in vertical {
                size = horizontal.size_vec2() * egui::Vec2::from(scale);
                let rect = egui::Rect::from_min_size(rect.min + egui::vec2(x, y), size);
                egui::Image::new(horizontal, size).paint_at(ui, rect);
                x += size.x;
            }

            x = 0.0;
            y += size.y;
        }
    }

    pub fn show(self, ui: &mut egui::Ui) -> egui::Response {
        let (rect, response) = ui.allocate_exact_size(self.size, self.sense);
        self.paint_at(ui, rect);
        response
    }
}

#[derive(Clone)]
pub enum TextureHandle {
    StaticTexture(SplittedTextureHandle),
    AnimatedTexture(AnimatedTextureHandle),
}

impl TextureHandle {
    pub fn from_image(
        tex_mgr: Arc<EguiRwLock<TextureManager>>,
        name: String,
        image: EguiSplittedImageData,
    ) -> Self {
        match image {
            EguiSplittedImageData::StaticImage(image) => {
                TextureHandle::StaticTexture(image.alloc(tex_mgr, name))
            }
            EguiSplittedImageData::AnimatedImage(frames) => TextureHandle::AnimatedTexture(
                AnimatedTextureHandle::from_data(tex_mgr, name, frames),
            ),
        }
    }

    pub fn into_view_state(self) -> TextureViewState {
        TextureViewState::from(self)
    }

    pub fn max_size(&self) -> [usize; 2] {
        match self {
            TextureHandle::StaticTexture(handle) => handle.size(),
            TextureHandle::AnimatedTexture(handle) => handle.max_size(),
        }
    }
}

#[derive(Clone)]
pub struct AnimatedTextureHandle {
    name: String,
    handles: Vec<TextureFrameHandle>,
}

impl AnimatedTextureHandle {
    pub fn new(name: String, handles: Vec<TextureFrameHandle>) -> Self {
        Self { name, handles }
    }

    pub fn from_data(
        tex_mgr: Arc<EguiRwLock<TextureManager>>,
        name: String,
        frames: Vec<EguiSplittedFrameData>,
    ) -> Self {
        let mut vec = Vec::new();
        for (index, frame) in frames.into_iter().enumerate() {
            let handle = TextureFrameHandle::from_frame(
                tex_mgr.clone(),
                format!("{}-{}", name, index),
                frame,
            );

            vec.push(handle);
        }

        Self::new(name, vec)
    }

    pub fn items(&self) -> &Vec<TextureFrameHandle> {
        &self.handles
    }

    pub fn max_size(&self) -> [usize; 2] {
        let mut size = [0usize; 2];
        for it in &self.handles {
            for i in 0..2 {
                size[i] = it.handle.size()[i].max(size[i]);
            }
        }

        size
    }
}

#[derive(Clone)]
pub struct TextureFrameHandle {
    delay: Duration,
    handle: SplittedTextureHandle,
}

impl TextureFrameHandle {
    pub fn from_frame(
        tex_mgr: Arc<EguiRwLock<TextureManager>>,
        name: String,
        frame: EguiSplittedFrameData,
    ) -> Self {
        let EguiSplittedFrameData { delay, image } = frame;
        let handle = image.alloc(tex_mgr, name);

        Self { delay, handle }
    }

    pub fn handle(&self) -> &SplittedTextureHandle {
        &self.handle
    }

    pub fn delay(&self) -> Duration {
        self.delay
    }
}

pub enum TextureViewState {
    StaticTexture(StaticTextureViewState),
    AnimatedTexture(AnimatedTextureViewState),
    Loading(LoadingTexture),
    Mutable(Arc<Mutex<Self>>),
}

impl TextureViewState {
    pub fn max_size(&self) -> [usize; 2] {
        match self {
            TextureViewState::StaticTexture(state) => state.handle.size(),
            TextureViewState::AnimatedTexture(state) => state.handles.max_size(),
            TextureViewState::Loading(loading) => loading.size_2(),
            TextureViewState::Mutable(state) => state.lock().max_size(),
        }
    }

    /// Returns `true` if the texture view state is [`Loading`].
    ///
    /// [`Loading`]: TextureViewState::Loading
    #[must_use]
    pub fn is_loading(&self) -> bool {
        matches!(self, Self::Loading(..))
    }

    /// Make self into loading, preserving its size.
    pub fn make_loading(&mut self) {
        let size = match self {
            TextureViewState::Mutable(state) => {
                state.lock().make_loading();
                return;
            }
            _ => self.max_size(),
        };

        let size = [size[0] as f32, size[1] as f32];

        *self = Self::Loading(LoadingTexture::new(self.name(), Some(size.into())));
        // *self = match *self {
        //     TextureViewState::StaticTexture(state) => Self::Loading(Lstate.handle.size_vec2(),
        //     TextureViewState::AnimatedTexture(_) => todo!(),
        //     TextureViewState::Loading(_) => todo!(),
        //     TextureViewState::Mutable(_) => todo!(),
        // }
    }

    pub fn name(&self) -> String {
        match self {
            TextureViewState::StaticTexture(state) => state.handle.name.clone(),
            TextureViewState::AnimatedTexture(state) => state.handles.name.clone(),
            TextureViewState::Loading(state) => state.name.clone(),
            TextureViewState::Mutable(state) => state.lock().name(),
            //
        }
    }
}

impl From<TextureHandle> for TextureViewState {
    fn from(texture: TextureHandle) -> Self {
        match texture {
            TextureHandle::AnimatedTexture(handles) => {
                Self::AnimatedTexture(AnimatedTextureViewState::new(handles))
            }
            TextureHandle::StaticTexture(handle) => {
                Self::StaticTexture(StaticTextureViewState { handle })
            }
        }
    }
}

pub struct StaticTextureViewState {
    pub handle: SplittedTextureHandle,
}

pub struct AnimatedTextureViewState {
    index: usize,
    handles: AnimatedTextureHandle,
    should_next: Arc<AtomicBool>,
    waker: crate::wake::EguiWaker,
}

impl AnimatedTextureViewState {
    pub fn new(handles: AnimatedTextureHandle) -> Self {
        Self {
            index: 0,
            handles,
            should_next: Arc::new(false.into()),
            waker: Default::default(),
        }
    }
    pub fn next_index(&mut self) {
        self.index = (self.index + 1) % (self.handles.items().len() - 1).max(1);
    }

    pub fn frame(&self) -> &TextureFrameHandle {
        &self.handles.items()[self.index]
    }
}

#[derive(Default)]
pub struct LoadingTexture {
    name: String,
    size: Option<egui::Vec2>,
}

impl LoadingTexture {
    pub fn new(name: String, size: Option<egui::Vec2>) -> Self {
        Self { name, size }
    }

    pub fn name(&self) -> &String {
        &self.name
    }

    pub fn size_2(&self) -> [usize; 2] {
        let size = self.size.unwrap_or([20.0, 20.0].into());

        [size.x as usize, size.y as usize]
    }

    // pub fn size(&self) -> usize {
    //     self.size.unwrap_or([20.0, 20.0].into())
    // }
}

pub struct TextureView<'a> {
    state: &'a mut TextureViewState,
}

impl<'a> TextureView<'a> {
    pub fn new(state: &'a mut TextureViewState) -> Self {
        Self { state }
    }

    pub fn show(
        self,
        ui: &mut egui::Ui,
        show_image: impl for<'b, 'c> Fn(
            &'b mut egui::Ui,
            &'c crate::image::SplittedTextureHandle,
        ) -> crate::image::SplittedTextureWidget<'c>,
    ) -> egui::Response {
        Self::show_state(self.state, ui, show_image)
    }

    fn show_state(
        state: &mut TextureViewState,
        ui: &mut egui::Ui,
        show_image: impl for<'b, 'c> Fn(
            &'b mut egui::Ui,
            &'c crate::image::SplittedTextureHandle,
        ) -> crate::image::SplittedTextureWidget<'c>,
    ) -> egui::Response {
        match state {
            TextureViewState::StaticTexture(state) => show_image(ui, &state.handle).show(ui),
            TextureViewState::AnimatedTexture(state) => {
                if state.should_next.load(Ordering::Relaxed) {
                    state.next_index();
                    state.should_next.store(false, Ordering::Relaxed);
                }

                let response = show_image(ui, state.frame().handle()).show(ui);

                if ui.is_rect_visible(response.rect) {
                    state.waker.call(state.frame().delay(), || {
                        let ctx = ui.ctx().clone();
                        let should_next = state.should_next.clone();

                        async move {
                            should_next.store(true, Ordering::Relaxed);
                            ctx.request_repaint();
                        }
                    });
                };

                response
            }

            TextureViewState::Loading(loading) => {
                let size = [
                    ui.available_width().max(loading.size_2()[0] as f32),
                    loading.size_2()[1] as f32,
                ];
                ui.allocate_ui(size.into(), |ui| {
                    ui.centered_and_justified(|ui| {
                        ui.add(egui::Label::new(format!("Loading {}", loading.name())));
                        // ui.add(egui::Spinner::new().size(loading.size() as f32));
                    })
                })
                .response
            }
            TextureViewState::Mutable(state) => Self::show_state(&mut state.lock(), ui, show_image),
        }
    }
}
