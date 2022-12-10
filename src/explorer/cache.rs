use std::{sync::Arc, collections::{HashMap, HashSet}};

use parking_lot::Mutex;
use serde::{Serialize, Deserialize};
use sha2::Digest;


pub fn sha_path(path: &std::path::Path) -> String {
    let sha = sha2::Sha256::digest(path.to_string_lossy().as_bytes());

    format!("{:x}", sha)
}

#[derive(Clone, Default, Debug, Serialize, Deserialize)]
pub struct ExplorerLoaderCache {
    pub map: Arc<Mutex<HashMap<String, String>>>,
    #[serde(default)]
    pub list: Arc<Mutex<HashMap<String, HashSet<String>>>>,
}

impl ExplorerLoaderCache {
    pub fn get_from_sha(&self, sha: &String) -> Option<String> {
        self.map.lock().get(sha).cloned()
    }

    pub fn get_from_path(&self, path: &std::path::Path) -> Option<String> {
        self.get_from_sha(&sha_path(path))
    }

    pub fn insert_sha(&self, path: &std::path::Path, target: String) {
        tracing::trace!("insert cache for {} using {}", path.display(), target);
        self.map.lock().insert(sha_path(path), target.clone());
        self.list
            .lock()
            .entry(target)
            .or_default()
            .insert(path.display().to_string());
    }

    pub fn insert_sha_recursive(
        &self,
        root: &std::path::Path,
        mut path: &std::path::Path,
        target: &str,
    ) {
        tracing::debug!(
            "insert sha recursive into [{:?}] [{:?}] [{}]",
            root,
            path,
            target,
        );

        let root = root.to_path_buf();

        while let Some(parent) = path.parent() {
            let should_break = Some(&root) == path.canonicalize().as_ref().ok();

            let is_root_file = || root.extension().is_some();
            // let is_root_file = || matches!(root.as_ref(), Ok(x) if x.extension().is_some());

            // if not should break should always insert
            // and it should also insert if root is file
            // it would'nt cache archive file otherwise.
            let should_insert = !should_break || is_root_file();

            if should_insert {
                self.insert_sha(path, target.to_string());
                path = parent;
            }

            if should_break {
                break;
            }
        }
    }
}
