use serde::{Deserialize, Serialize};
use std::{
    io::{BufRead, Cursor, Read, Seek},
    path::Path,
    sync::Arc,
    time::Duration,
};

mod reader;
mod texture;

use eframe::egui;
use eframe::epaint::mutex::RwLock as EguiRwLock;
use eframe::epaint::TextureManager;
pub use reader::*;
pub use texture::*;

use image::AnimationDecoder;

#[derive(Clone, Copy, PartialEq, Eq, Debug, Default, Serialize, Deserialize)]
pub enum FilterType {
    /// Nearest Neighbor
    #[default]
    Nearest,

    /// Linear Filter
    Triangle,

    /// Cubic Filter
    CatmullRom,

    /// Gaussian Filter
    Gaussian,

    /// Lanczos with window 3
    Lanczos3,
}

impl From<FilterType> for image::imageops::FilterType {
    fn from(value: FilterType) -> Self {
        use image::imageops::FilterType as F;
        match value {
            FilterType::Nearest => F::Nearest,
            FilterType::Triangle => F::Triangle,
            FilterType::CatmullRom => F::CatmullRom,
            FilterType::Gaussian => F::Gaussian,
            FilterType::Lanczos3 => F::Lanczos3,
        }
    }
}

pub trait ToEguiImage {
    fn to_egui_image(&self) -> egui::ImageData;
}

impl ToEguiImage for image::DynamicImage {
    fn to_egui_image(&self) -> egui::ImageData {
        let size = [self.width() as _, self.height() as _];
        let image_buffer = self.to_rgba8();
        let pixels = image_buffer.as_flat_samples();

        let color = egui::ColorImage::from_rgba_unmultiplied(size, pixels.as_slice());
        egui::ImageData::Color(color.into())
    }
}

pub enum ImageData {
    StaticImage(image::DynamicImage),
    AnimatedImage(Vec<FrameData>),
}

impl ImageData {
    /// Resize this image using the specified filter algorithm.
    /// Returns a new image. The image's aspect ratio is preserved.
    /// The image is scaled to the maximum possible size that fits
    /// within the bounds specified by `nwidth` and `nheight`.
    pub fn resize(&self, nwidth: u32, nheight: u32, filter: FilterType) -> Self {
        match self {
            Self::StaticImage(image) => {
                Self::StaticImage(image.resize(nwidth, nheight, filter.into()))
            }
            Self::AnimatedImage(frames) => {
                let mut new = vec![];
                for it in frames {
                    new.push(FrameData {
                        image: it.image.resize(nwidth, nheight, filter.into()),
                        top: it.top,
                        left: it.left,
                        delay: it.delay,
                    });
                }
                Self::AnimatedImage(new)
            }
        }
    }

    /// Split image by size (width, height) to make them allocatable
    pub fn into_allocatable(self, max_size: (u32, u32)) -> SplittedImageData {
        match self {
            ImageData::StaticImage(image) => {
                SplittedImageData::StaticImage(SplittedImage::make_allocatable(&image, max_size))
            }
            ImageData::AnimatedImage(images) => SplittedImageData::AnimatedImage(
                images
                    .into_iter()
                    .map(|it| SplittedFrameData {
                        delay: it.delay.into(),
                        image: SplittedImage::make_allocatable(&it.image, max_size),
                    })
                    .collect(),
            ),
        }
    }

    pub fn write_to<W, F>(&self, w: &mut W, static_format: F) -> image::ImageResult<()>
    where
        W: std::io::Write + Seek,
        F: Into<image::ImageOutputFormat>,
    {
        match self {
            ImageData::StaticImage(image) => {
                image.write_to(w, static_format)?;
            }
            ImageData::AnimatedImage(image) => {
                // let mut encode = gif::Encoder;
                let mut encode = image::codecs::gif::GifEncoder::new(w);
                // encode.set_repeat(image::codecs::gif::Repeat::Infinite)?;

                for it in image {
                    let image = it.image.to_rgba8();
                    let frame = image::Frame::from_parts(image, it.top, it.left, it.delay);
                    encode.encode_frame(frame)?;
                }
            }
        };

        Ok(())
    }
}

impl ImageData {
    pub fn can_read(name: &str) -> bool {
        image::ImageFormat::from_path(name)
            .map(|it| it.can_read())
            .unwrap_or(false)
    }

    pub fn open(path: impl AsRef<Path>) -> image::ImageResult<Self> {
        let image = match image::ImageFormat::from_path(&path) {
            Ok(image::ImageFormat::Gif) => Self::load_gif(std::fs::File::open(path)?)?,
            // guess format from memory
            Ok(_) | Err(image::ImageError::Unsupported(_)) => {
                let image = image::io::Reader::open(path)?
                    .with_guessed_format()?
                    .decode()?;

                ImageData::StaticImage(image)
            }
            Err(err) => return Err(err),
        };

        Ok(image)
    }

    pub fn load_gif(read: impl Read) -> image::ImageResult<Self> {
        let decoder = image::codecs::gif::GifDecoder::new(read)?;

        let frames = decoder
            .into_frames()
            .collect_frames()?
            .into_iter()
            .map(FrameData::from)
            .collect::<Vec<_>>();

        Ok(ImageData::AnimatedImage(frames))
    }

    pub fn load(mut read: impl BufRead + Seek) -> image::ImageResult<Self> {
        let format = image::io::Reader::new(&mut read).with_guessed_format()?;
        let format = format.format();

        if let Some(format) = format {
            Self::load_with_format(read, format)
        } else {
            todo!();
        }
    }

    pub fn load_from_name(name: &str, mut read: impl Read) -> image::ImageResult<Self> {
        let format = image::ImageFormat::from_path(std::path::PathBuf::from(name))?;

        let mut reader = Vec::new();
        read.read_to_end(&mut reader)?;

        ImageData::load_with_format(Cursor::new(reader), format)
    }

    pub fn load_with_format(
        read: impl BufRead + Seek,
        format: image::ImageFormat,
    ) -> image::ImageResult<Self> {
        let image = match format {
            image::ImageFormat::Gif => {
                let decoder = image::codecs::gif::GifDecoder::new(read)?;

                let frames = decoder.into_frames().collect_frames()?;

                let frames = frames.into_iter().map(FrameData::from).collect();

                ImageData::AnimatedImage(frames)
            }

            _ => {
                let image = image::io::Reader::with_format(read, format).decode()?;

                ImageData::StaticImage(image)
            }
        };

        Ok(image)
    }

    pub fn load_from_memory(data: &[u8]) -> image::ImageResult<Self> {
        let format = image::guess_format(data)?;

        Self::load_from_memory_with_format(data, format)
    }

    pub fn load_from_memory_with_format(
        data: &[u8],
        format: image::ImageFormat,
    ) -> image::ImageResult<Self> {
        let image = match format {
            image::ImageFormat::Gif => Self::load_gif(data)?,
            format => {
                let image = image::load_from_memory_with_format(data, format)?;

                Self::StaticImage(image)
            }
        };

        Ok(image)
    }
}

pub struct FrameData {
    delay: image::Delay,
    top: u32,
    left: u32,
    image: image::DynamicImage,
}

impl std::fmt::Debug for FrameData {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        f.debug_struct("ImageFrame")
            .field("delay", &self.delay)
            // .field("image", &self.image)
            .finish()
    }
}

impl From<image::Frame> for FrameData {
    fn from(frame: image::Frame) -> Self {
        Self {
            delay: frame.delay(),
            top: frame.top(),
            left: frame.left(),
            image: image::DynamicImage::from(frame.into_buffer()),
        }
    }
}

pub fn get_allocatable_area(
    image: &image::DynamicImage,
    size: (u32, u32),
) -> Vec<Vec<(u32, u32, u32, u32)>> {
    let (mut x, mut y) = (0, 0);

    let mut pos = 0;
    let mut vec = vec![];

    while x < image.width() && y < image.height() {
        let width = (image.width() - x).min(size.0);
        let height = (image.height() - y).min(size.1);

        vec.resize(pos + 1, vec![]);

        vec[pos].push((x, y, width, height));

        x += width;

        if x >= image.width() {
            x = 0;
            y += height;

            pos += 1;
        }
    }

    vec
}

/// Make DynamicImage allocateable by spliting them
/// the splited image is arranged as vertical<horizontal<image>>
pub fn make_allocatable(
    image: &image::DynamicImage,
    max_size: (u32, u32),
) -> Vec<Vec<image::DynamicImage>> {
    get_allocatable_area(image, max_size)
        .into_iter()
        .map(|vec| {
            vec.into_iter()
                .map(|(x, y, width, height)| image.crop_imm(x, y, width, height))
                .collect()
        })
        .collect()
}

#[derive(Clone)]
pub struct SplittedImage(Vec<Vec<image::DynamicImage>>);

impl SplittedImage {
    pub fn new(image: Vec<Vec<image::DynamicImage>>) -> Self {
        Self(image)
    }

    pub fn make_allocatable(image: &image::DynamicImage, max_size: (u32, u32)) -> Self {
        Self::new(make_allocatable(image, max_size))
    }

    pub fn into_egui_image(self) -> EguiSplittedStaticImageData {
        EguiSplittedStaticImageData::from(self)
    }
}

#[derive(Clone)]
pub struct SplittedFrameData {
    delay: Duration,
    image: SplittedImage,
}

#[derive(Clone)]
pub enum SplittedImageData {
    StaticImage(SplittedImage),
    AnimatedImage(Vec<SplittedFrameData>),
}

impl SplittedImageData {
    pub fn into_egui(self) -> EguiSplittedImageData {
        EguiSplittedImageData::from(self)
    }
}

#[derive(Clone)]
pub struct EguiSplittedStaticImageData(Vec<Vec<egui::ImageData>>);

impl From<SplittedImage> for EguiSplittedStaticImageData {
    fn from(image: SplittedImage) -> Self {
        Self(
            image
                .0
                .into_iter()
                .map(|vec| vec.into_iter().map(|it| it.to_egui_image()).collect())
                .collect(),
        )
    }
}

pub struct EguiSplittedFrameData {
    delay: Duration,
    image: EguiSplittedStaticImageData,
}

impl From<SplittedFrameData> for EguiSplittedFrameData {
    fn from(frame: SplittedFrameData) -> Self {
        Self {
            delay: frame.delay,
            image: EguiSplittedStaticImageData::from(frame.image),
        }
    }
}

pub enum EguiSplittedImageData {
    StaticImage(EguiSplittedStaticImageData),
    AnimatedImage(Vec<EguiSplittedFrameData>),
}

impl EguiSplittedImageData {
    pub fn alloc(
        self,
        tex_mgr: Arc<eframe::epaint::mutex::RwLock<TextureManager>>,
        name: String,
        option: TextureOption,
    ) -> texture::TextureHandle {
        TextureHandle::from_image(tex_mgr, name, self, option)
    }
}

impl From<SplittedImageData> for EguiSplittedImageData {
    fn from(image: SplittedImageData) -> Self {
        match image {
            SplittedImageData::StaticImage(image) => {
                Self::StaticImage(EguiSplittedStaticImageData::from(image))
            }
            SplittedImageData::AnimatedImage(image) => {
                //
                Self::AnimatedImage(image.into_iter().map(EguiSplittedFrameData::from).collect())
            }
        }
    }
}

impl EguiSplittedStaticImageData {
    pub fn alloc(
        self,
        tex_mgr: Arc<EguiRwLock<TextureManager>>,
        name: String,
        option: TextureOption,
    ) -> SplittedTextureHandle {
        let vec = self
            .0
            .into_iter()
            .enumerate()
            .map(|(i, it)| {
                it.into_iter()
                    .enumerate()
                    .map(|(j, it)| {
                        let id =
                            tex_mgr
                                .write()
                                .alloc(format!("{}-{}x{}", name, i, j), it, option.0);
                        egui::TextureHandle::new(tex_mgr.clone(), id)
                    })
                    .collect()
            })
            .collect();

        SplittedTextureHandle::new(name, vec)
    }
}
