use futures::stream::StreamExt;
use image::imageops::FilterType;
use std::{
    path::{Path, PathBuf},
    time::Instant,
};
use tokio::task::yield_now;
use zip::read::ZipArchive;

use crate::image::{EguiSplittedImageData, ImageData};

pub async fn search_image(ctx: egui::Context, path: PathBuf) -> Option<EguiSplittedImageData> {
    if path.exists() {
        let dir = walkdir::WalkDir::new(path)
            .same_file_system(true)
            .sort_by(|a, b| crate::path::compare_natural(a.file_name(), b.file_name()));

        let dir = dir.into_iter().filter_map(|it| it.ok());

        let dir = futures::stream::unfold(dir, |mut dir| async {
            tokio::task::spawn_blocking(|| {
                let next = dir.next();

                next.map(|next| (next, dir))
            })
            .await
            .ok()
            .unwrap()
        });
        futures::pin_mut!(dir);

        let size = ctx.input(|input| input.max_texture_side) as u32;

        while let Some(entry) = dir.next().await {
            let entry: walkdir::DirEntry = entry;

            if entry.file_type().is_file() {
                if let Some(image) = load_path(entry.path().to_path_buf(), size).await {
                    return Some(image);
                }
            }
        }
    }

    None
}

pub async fn load_path(path: PathBuf, size: u32) -> Option<EguiSplittedImageData> {
    let time = Instant::now();
    log::debug!("opening {:?}", path);
    let mut image = None;

    if let Ok(reader) = crate::image::Reader::open(&path) {
        image = reader
            .into_frames()
            .into_collector()
            .collect_as_option_vector()
            .await;
    }

    let image = match image {
        Some(image) => image,
        None => match search_file(&path).await {
            Some(image) => image,
            None => return None,
        },
    };

    log::trace!("loading image {:?} in {:?}", path, time.elapsed());

    yield_now().await;
    let image = image.resize(500, 500, FilterType::Triangle);
    log::trace!("resize image {:?} in {:?}", path, time.elapsed());

    yield_now().await;
    let image = image.into_allocatable((size, size));
    log::trace!("splitting image {:?} in {:?}", path, time.elapsed());

    yield_now().await;
    let image = EguiSplittedImageData::from(image);
    log::trace!("into egui {:?} in {:?}", path, time.elapsed());

    Some(image)
}

pub async fn search_file(path: impl AsRef<Path>) -> Option<ImageData> {
    let file = std::fs::File::open(&path).ok()?;
    if let Ok(mut zip) = ZipArchive::new(file) {
        for i in 0..zip.len() {
            if let Some(image) = crate::tools::zip::load_image(|| zip.by_index(i)).await {
                return Some(image);
            }
        }
    }
    None
}
