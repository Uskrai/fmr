use std::{
    fs::File,
    io::{BufRead, BufReader, Cursor, Read, Seek},
    path::Path,
};

use image::AnimationDecoder;

use crate::ImageData;

pub struct ReaderBuilder {
    video: bool,
    animated: bool,
    writable: bool,
}

impl Default for ReaderBuilder {
    fn default() -> Self {
        Self {
            video: true,
            animated: true,
            writable: false,
        }
    }
}

impl ReaderBuilder {
    // only open if file is writable as image.
    pub fn writable(mut self, enable: bool) -> Self {
        self.writable = enable;
        self
    }

    pub fn animated(mut self, enable: bool) -> Self {
        self.animated = enable;
        self
    }

    pub fn video(mut self, enable: bool) -> Self {
        self.video = enable;
        self
    }

    pub fn open(
        self,
        path: impl AsRef<Path>,
    ) -> Result<Reader<BufReader<File>>, image::ImageError> {
        let load_gif = || Reader::load_gif(BufReader::new(std::fs::File::open(&path)?));
        let ext = path.as_ref().extension().and_then(|it| it.to_str());

        let is_video = matches!(ext, Some("mp4" | "mkv"));

        let load_memory = || {
            let reader = image::io::Reader::open(&path)?.with_guessed_format()?;

            match reader.format() {
                Some(image::ImageFormat::Gif) if self.animated => load_gif(),
                // can't write video as image so check if isn't writable
                None if is_video && !self.writable => {
                    Ok(Reader::VideoPath(path.as_ref().to_path_buf()))
                }
                _ => Ok(Reader::Reader(Box::new(reader))),
            }
        };

        let reader = match image::ImageFormat::from_path(&path) {
            Ok(image::ImageFormat::Gif) => load_gif()?,
            // guess format from memory
            Ok(_) | Err(image::ImageError::Unsupported(_)) => load_memory()?,
            Err(err) => {
                return Err(err);
            }
        };

        Ok(reader)
    }
}

pub enum Reader<T: Read> {
    Reader(Box<image::io::Reader<T>>),
    VideoPath(std::path::PathBuf),
    Gif(Box<image::codecs::gif::GifDecoder<T>>),
}

impl Reader<BufReader<File>> {
    pub fn builder() -> ReaderBuilder {
        ReaderBuilder::default()
    }

    pub fn open(path: impl AsRef<Path>) -> Result<Self, image::ImageError> {
        ReaderBuilder::default().open(path)
    }
}

impl<T: AsRef<[u8]>> Reader<Cursor<T>> {
    pub fn load_from_memory(memory: T) -> Result<Self, image::ImageError> {
        let reader = match image::guess_format(memory.as_ref())? {
            image::ImageFormat::Gif => Self::load_gif(Cursor::new(memory))?,
            format => {
                let reader = image::io::Reader::with_format(Cursor::new(memory), format)
                    .with_guessed_format()?;
                Self::Reader(Box::new(reader))
            }
        };

        Ok(reader)
    }
}

impl<T: Read> Reader<T> {
    pub fn load_gif(read: T) -> Result<Self, image::ImageError> {
        let decoder = image::codecs::gif::GifDecoder::new(read)?;

        Ok(Self::Gif(Box::new(decoder)))
    }
}

impl<'a, T: Read + 'a> Reader<T> {
    pub fn into_frames(self) -> Frames<'a, T> {
        match self {
            Reader::Reader(reader) => Frames::SingleImage(reader),
            Reader::Gif(frames) => Frames::Frames(frames.into_frames()),
            Reader::VideoPath(path) => Frames::VideoPath(path),
        }
    }
}

pub enum Frames<'a, T: Read + 'a> {
    SingleImage(Box<image::io::Reader<T>>),
    Frames(image::Frames<'a>),
    VideoPath(std::path::PathBuf),
}

impl<'a, T: BufRead + Seek> Frames<'a, T> {
    pub fn into_collector(self) -> FramesCollector<'a, T> {
        match self {
            Self::SingleImage(reader) => {
                FramesCollector::SingleImage(SingleImageEither::Reader(Some(reader)))
            }
            Self::Frames(frames) => FramesCollector::Frames {
                frames,
                collected: Vec::new(),
            },
            Self::VideoPath(path) => FramesCollector::VideoPath(path),
        }
    }
}

pub enum SingleImageEither<T: Read> {
    Reader(Option<Box<image::io::Reader<T>>>),
    Image(image::DynamicImage),
}

pub enum FramesCollector<'a, T: Read + 'a> {
    SingleImage(
        SingleImageEither<T>, // reader: Option<image::io::Reader<T>>,
                              // image: Option<
    ),
    Frames {
        frames: image::Frames<'a>,
        collected: Vec<super::FrameData>,
    },
    VideoPath(std::path::PathBuf),
}

unsafe impl<'a, T: Read + Send + 'a> Send for Frames<'a, T> {}
unsafe impl<'a, T: Read + Send + 'a> Send for FramesCollector<'a, T> {}

impl<'a, T: BufRead + Seek> FramesCollector<'a, T> {
    pub fn load_next(&mut self) -> Result<bool, image::ImageError> {
        match self {
            FramesCollector::SingleImage(image) => {
                if let SingleImageEither::Reader(reader) = image {
                    if let Some(reader) = reader.take() {
                        let time = std::time::Instant::now();
                        *image = SingleImageEither::Image(reader.decode()?);
                        tracing::info!("decoding image in {:?}", time.elapsed());
                    }
                }

                Ok(false)
            }
            FramesCollector::Frames { frames, collected } => {
                if let Some(image) = frames.next() {
                    let image = image?;
                    collected.push(image.into());

                    Ok(true)
                } else {
                    Ok(false)
                }
            }
            FramesCollector::VideoPath(_) => {
                tracing::info!("loading video");
                Ok(false)
            }
        }
    }

    pub fn into_image(self) -> Option<ImageData> {
        match self {
            FramesCollector::SingleImage(image) => match image {
                SingleImageEither::Reader(_) => None,
                SingleImageEither::Image(image) => Some(ImageData::StaticImage(image)),
            },
            FramesCollector::Frames { collected, .. } => Some(ImageData::AnimatedImage(collected)),
            FramesCollector::VideoPath(path) => Some(ImageData::VideoPath(path)),
        }
    }

    pub async fn collect_vector(mut self) -> Option<Result<ImageData, image::ImageError>> {
        loop {
            match self.load_next() {
                Ok(true) => tokio::task::yield_now().await,
                Ok(false) => return self.into_image().map(Ok),
                Err(err) => return Some(Err(err)),
            }
        }
    }

    pub async fn collect_as_option_vector(self) -> Option<ImageData> {
        self.collect_vector().await.transpose().ok().flatten()
    }
}
