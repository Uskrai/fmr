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

#ifndef FMR_FILE_HANDLER_FILESYSTEM_STREAM
#define FMR_FILE_HANDLER_FILESYSTEM_STREAM

#include <fmr/common/path.h>
#include <fmr/file_handler/local/stream.h>
#include <fmr/nowide/filesystem.h>
#include <fmr/nowide/fstream.h>

#include <memory>
#include <vector>

namespace fmr {

namespace file_handler {

namespace filesystem {

using StreamBase = file_handler::local::Stream;
class Stream : public StreamBase {
  nwd::fs::path path_;
  std::string filename_;
  nwd::fstream file_stream_;
  std::shared_ptr<std::vector<char>> buffer_;

 public:
  Stream(const Stream &stream);
  Stream(Stream &&stream);

  Stream(const nwd::fs::path &path);
  Stream(nwd::fs::path &&path);

  Stream(const std::string &path) : Stream(Path::MakePath(path)) {}
  Stream(const char *path) : Stream(Path::MakePath(path)) {}

  std::string GetName() override { return filename_; }
  const std::string &GetString() override { return filename_; }
  std::string GetHandlerPath() override { return path_.parent_path(); }

  std::string GetFullPath() const override { return Path::MakeString(path_); }

  bool IsShared() const override { return buffer_.use_count() != 0; }

  bool IsDirectory() const override { return Path::IsDirectory(path_); }

  size_t Size() const override;

  void *GetBuffer() override { return buffer_->data(); }
  const void *GetBuffer() const override { return buffer_->data(); }

  Stream *Clone() const override { return new Stream(*this); }
};

}  // namespace filesystem

}  // namespace file_handler

}  // namespace fmr

#endif /* end of include guard: FMR_FILE_HANDLER_FILESYSTEM_STREAM */
