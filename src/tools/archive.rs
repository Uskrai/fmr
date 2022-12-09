use std::{io::Read, path::PathBuf};

use futures::Future;
use libarchive::reader::{ArchiveEntry, ReaderHandle};
use tap::Tap;

use crate::image::ImageData;

use super::image::load_image_from_memory_as_option;

pub fn can_read(path: &PathBuf) -> bool {
    open(&path).is_ok()
}

pub fn open(path: &PathBuf) -> Result<ReaderHandle, libarchive::error::ArchiveError> {
    libarchive::reader::Builder::new()
        .support_all()
        .and_then(|it| it.open_file(path.clone()))
}

pub fn is_image(entry: &ArchiveEntry) -> bool {
    let name = entry.pathname();
    let isfile = entry.is_file();

    let should = isfile
        && name
            .map(|pathname| crate::image::ImageData::can_read(&pathname))
            .unwrap_or(false);
    should
}

pub fn read_to_end_when(
    mut entry: ArchiveEntry,
    when: impl FnOnce(&ArchiveEntry) -> bool,
) -> Option<Vec<u8>> {
    if when(&entry) {
        let mut vec = Vec::new();
        entry.read_to_end(&mut vec).ok()?;
        Some(vec)
    } else {
        None
    }
}

pub fn read_to_end_when_image(zip: ArchiveEntry) -> Option<Vec<u8>> {
    read_to_end_when(zip, is_image)
}

pub fn load_image_from_entry(zip: ArchiveEntry) -> impl Future<Output = Option<ImageData>> + Send {
    let time = std::time::Instant::now();
    let it = read_to_end_when_image(zip);
    tracing::trace!("reading memory from entry in {:?}", time.elapsed());

    async { load_image_from_memory_as_option(it).await }
}

pub async fn flatten_future<F, O>(f: Option<F>) -> Option<O>
where
    F: Future<Output = Option<O>>,
{
    if let Some(it) = f {
        it.await
    } else {
        None
    }
}

pub fn load_image_from_opt_entry(
    zip: Option<ArchiveEntry>,
) -> impl Future<Output = Option<ImageData>> + Send {
    let it = zip.map(|zip| load_image_from_entry(zip));

    flatten_future(it)
}

pub fn load_path_to_image(
    reader: ReaderHandle,
    name: String,
) -> impl Future<Output = Option<ImageData>> + Send {
    let time = std::time::Instant::now();

    let it = reader.into_iter().filter_map(|it| it.ok()).find(|it| {
        let s = it.pathname() == Some(name.clone());
        s
    });
    tracing::trace!("finding archive entry in {:?}", time.elapsed());

    load_image_from_opt_entry(it)
}

pub fn load_path_to_image_from_opt(
    reader: Option<ReaderHandle>,
    name: String,
) -> impl Future<Output = Option<ImageData>> + Send {
    let it = reader.map(|it| load_path_to_image(it, name));

    flatten_future(it)
}
