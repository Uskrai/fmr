/*
 *  Copyright (c) 2021 Uskrai
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "fmr/file_handler/filesystem/read_stream.h"

#include "fmr/common/path.h"

namespace fmr {

namespace file_handler {

namespace filesystem {

// Stream::Stream(Stream &&stream) {}

// Stream::Stream(const Stream &stream) = default;

ReadStream::ReadStream(const nwd::fs::path &path, bool load)
    : path_(path), filename_(Path::MakeString(path.filename())) {
  if (load) Load();
}

size_t ReadStream::FileSize() const {
  return IsDirectory() ? 0 : nwd::fs::file_size(path_);
}

bool ReadStream::Load() {
  size_t size = FileSize();

  auto vec = std::make_shared<std::vector<std::byte>>(size);
  vec->resize(size);

  nwd::ifstream stream(Path::MakeString(path_));
  stream.read(reinterpret_cast<char *>(vec->data()), size);

  stream_.Clear();
  stream_.Write(vec);

  loaded_ = true;
  return true;
}

size_t ReadStream::Size() const { return stream_.Size(); }

}  // namespace filesystem

}  // namespace file_handler

}  // namespace fmr
