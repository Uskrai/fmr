use std::{io::Read, path::PathBuf};

#[derive(Debug)]
pub struct FSStorage {
    path: PathBuf,
    content: String,
    dirty: bool,

    last_write_join_handle: Option<std::thread::JoinHandle<()>>,
}

impl FSStorage {
    pub fn prepare(name: &str, profile: &str, filename: &str) -> Option<Self> {
        if let Some(project_dirs) = directories_next::ProjectDirs::from("", "", name) {
            let profile_dir = project_dirs.config_dir().join(profile);

            if let Err(err) = std::fs::create_dir_all(&profile_dir) {
                log::warn!(
                    "Config disabled: failed to create directory at {:?}: {}",
                    profile_dir,
                    err
                );
                None
            } else {
                let path = profile_dir.join(filename);

                fn inner(path: &PathBuf) -> std::io::Result<String> {
                    let mut file = std::fs::File::options()
                        .create(true)
                        .write(true)
                        .read(true)
                        .open(path)?;
                    let mut string = String::new();
                    file.read_to_string(&mut string)?;
                    Ok(string)
                }

                let content = match inner(&path) {
                    Ok(c) => c,
                    Err(e) => {
                        log::warn!("Failed reading config at {:?}: {}", path, e);
                        return None;
                    }
                };

                Some(Self {
                    path,
                    dirty: false,
                    content,
                    last_write_join_handle: None,
                })
            }
        } else {
            None
        }
    }

    pub fn set_content(&mut self, content: String) {
        if content != self.content {
            self.dirty = true;
            self.content = content;
        }
    }

    pub fn get_content(&self) -> &str {
        &self.content
    }

    pub fn flush(&mut self) {
        if self.dirty {
            let content = self.content.clone();

            self.dirty = false;
            self.flush_content(|| Some(content));
        }
    }

    pub fn flush_content<F>(&mut self, content: F)
    where
        F: FnOnce() -> Option<String> + Send + 'static,
    {
        let path = self.path.clone();

        let mut tmp_ext = path.extension().unwrap_or_default().to_os_string();
        tmp_ext.push(".tmp");

        let tmp_path = path.with_extension(tmp_ext);

        let last_write = self.last_write_join_handle.take();

        let handle = std::thread::spawn(move || {
            if let Some(join_handle) = last_write {
                // wait previous write.
                join_handle.join().ok();
            }

            while let Ok(metadata) = std::fs::metadata(&tmp_path) {
                if let Some(elapsed) = metadata
                    .created()
                    .map(|it| it.elapsed().ok())
                    .ok()
                    .flatten()
                {
                    if elapsed > std::time::Duration::from_secs(10) {
                        break;
                    }
                }

                std::thread::sleep(std::time::Duration::from_secs(1));
            }

            if let Some(content) = content() {
                log::debug!("Writing config to {:?}", path);
                if let Err(err) = std::fs::write(&tmp_path, content) {
                    log::warn!("Writing config to {:?} failed: {}", path, err);
                } else {
                    if let Err(err) = std::fs::rename(&tmp_path, &path) {
                        log::warn!("Renaming config from {tmp_path:?} to {path:?} failed: {err}");
                    } else {
                        log::debug!("Config written at {:?}", path);
                    }
                }
            }
        });

        self.last_write_join_handle = Some(handle);
    }
}
