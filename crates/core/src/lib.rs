pub mod path;

/// RAII that abort the handle when dropped.
pub struct AbortOnDropHandle<T>(pub tokio::task::JoinHandle<T>);

impl<T> std::future::Future for AbortOnDropHandle<T> {
    type Output = <tokio::task::JoinHandle<T> as std::future::Future>::Output;

    fn poll(
        mut self: std::pin::Pin<&mut Self>,
        cx: &mut std::task::Context<'_>,
    ) -> std::task::Poll<Self::Output> {
        use futures::FutureExt;
        self.0.poll_unpin(cx)
    }
}

pub fn spawn_and_abort_on_drop<F>(fut: F) -> AbortOnDropHandle<F::Output>
where
    F: std::future::Future + Send + 'static,
    F::Output: Send + 'static,
{
    AbortOnDropHandle(tokio::spawn(fut))
}

// impl RangeExt for std::ops::Range<usize> {
pub fn split_and_interleave_at(
    range: std::ops::Range<usize>,
    index: usize,
) -> itertools::Interleave<std::ops::Range<usize>, std::iter::Rev<std::ops::Range<usize>>> {
    use itertools::Itertools;
    (index..range.end).interleave((range.start..index).rev())
}

// pub use app::{App, AppMode};
// use parking_lot::Mutex;
