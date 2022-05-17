use std::io::Read;

use zip::{read::ZipFile, ZipArchive};

use crate::image::ImageData;

pub fn read_to_end_when(mut zip: ZipFile, when: impl FnOnce(&ZipFile) -> bool) -> Option<Vec<u8>> {
    if when(&zip) {
        let mut vec = Vec::new();
        zip.read_to_end(&mut vec).ok()?;
        Some(vec)
    } else {
        None
    }
}

pub fn read_to_end_when_image(zip: ZipFile) -> Option<Vec<u8>> {
    read_to_end_when(zip, |it| {
        it.is_file() && crate::image::ImageData::can_read(it.name())
    })
}

#[inline]
pub fn load_image<'a, F>(
    zip: F,
) -> impl std::future::Future<Output = Option<ImageData>> + Send + 'static
where
    F: FnOnce() -> zip::result::ZipResult<ZipFile<'a>>,
{
    let vec = zip().ok().and_then(read_to_end_when_image);
    load_image_from_memory_as_option(vec)
}

pub fn load_image_with_ref<F, Z>(
    archive: &mut ZipArchive<Z>,
    zip: F,
) -> impl std::future::Future<Output = Option<ImageData>> + Send + 'static
where
    F: FnOnce(&mut ZipArchive<Z>) -> zip::result::ZipResult<ZipFile<'_>>,
    Z: Read,
{
    let vec = zip(archive).ok().and_then(read_to_end_when_image);

    load_image_from_memory_as_option(vec)
}

async fn load_image_from_memory_as_option(vec: Option<Vec<u8>>) -> Option<ImageData> {
    load_image_from_memory(vec).await.transpose().ok().flatten()
}

async fn load_image_from_memory(
    vec: Option<Vec<u8>>,
) -> Option<Result<ImageData, image::ImageError>> {
    let vec = vec?;
    tokio::task::yield_now().await;
    let vec = vec;

    let reader = match crate::image::Reader::load_from_memory(vec) {
        Ok(reader) => reader,
        Err(err) => {
            return Some(Err(err));
        }
    };

    reader.into_frames().into_collector().collect_vector().await
}
