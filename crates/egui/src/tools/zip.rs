use std::io::Read;

use zip::{read::ZipFile, ZipArchive};

use fmr_frame::ImageData;

use super::image::load_image_from_memory_as_option;

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
    read_to_end_when(zip, |it| it.is_file() && ImageData::can_read(it.name()))
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

#[tracing::instrument(skip(archive, zip))]
pub fn load_image_with_ref<F, Z>(
    archive: &mut ZipArchive<Z>,
    zip: F,
) -> impl std::future::Future<Output = Option<ImageData>> + Send + 'static
where
    F: FnOnce(&mut ZipArchive<Z>) -> zip::result::ZipResult<ZipFile<'_>>,
    Z: Read,
{
    let time = std::time::Instant::now();
    let vec = zip(archive).ok().and_then(read_to_end_when_image);
    tracing::trace!("loading memory from zip in {:?}", time.elapsed());

    async {
        let res = load_image_from_memory_as_option(vec).await;
        tracing::trace!("loading image from memory in {:?}", time.elapsed());

        res
    }
}
