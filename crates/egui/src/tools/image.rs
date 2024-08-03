use fmr_frame::ImageData;

pub async fn load_image_from_memory_as_option(vec: Option<Vec<u8>>) -> Option<ImageData> {
    load_image_from_memory(vec).await.transpose().ok().flatten()
}

#[tracing::instrument(skip(vec))]
pub async fn load_image_from_memory(
    vec: Option<Vec<u8>>,
) -> Option<Result<ImageData, image::ImageError>> {
    let vec = vec?;
    tokio::task::yield_now().await;
    let time = std::time::Instant::now();

    let reader = match fmr_frame::Reader::load_from_memory(vec) {
        Ok(reader) => reader,
        Err(err) => {
            return Some(Err(err));
        }
    };
    tracing::trace!("create reader from memory in {:?}", time.elapsed());

    let vector = reader.into_frames().into_collector().collect_vector().await;
    tracing::trace!("converting reader into frames in {:?}", time.elapsed());

    vector
}
