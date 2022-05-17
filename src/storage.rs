use std::{io::Read, path::PathBuf};

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
            self.dirty = false;

            let path = self.path.clone();
            let content = self.content.clone();

            if let Some(join_handle) = self.last_write_join_handle.take() {
                // wait previous write.
                join_handle.join().ok();
            }

            let handle = std::thread::spawn(move || {
                if let Err(err) = std::fs::write(&path, content) {
                    log::warn!("Writing config to {:?} failed: {}", path, err);
                } else {
                    log::trace!("Config written at {:?}", path);
                }
            });

            self.last_write_join_handle = Some(handle);
        }
    }
}
