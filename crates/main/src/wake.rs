use std::{sync::Arc, time::Duration};

use parking_lot::Mutex;

/// call future after specified duration
#[derive(Default)]
pub struct EguiWaker {
    pub handle: Arc<Mutex<Option<tokio::task::JoinHandle<()>>>>,
}

// impl Default for EguiWaker {
//     fn default() -> Self {
//         Self {
//             handle: Default::default(),
//         }
//     }
// }

impl EguiWaker {
    pub fn call<F, R>(&mut self, duration: Duration, call: F)
    where
        F: FnOnce() -> R,
        R: std::future::Future + Send + Sync + 'static,
    {
        let handle = self.handle.clone();
        let opt = handle.lock().take();

        *self.handle.lock() = match opt {
            Some(handle) => Some(handle),
            None => Some(tokio::spawn({
                let fut = call();
                async move {
                    tokio::time::sleep(duration).await;
                    fut.await;
                    *handle.lock() = None;
                }
            })),
        };
    }
}
