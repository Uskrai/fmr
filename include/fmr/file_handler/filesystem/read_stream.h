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
#include <fmr/file_handler/local/read_stream.h>
#include <fmr/file_handler/memory_stream.h>
#include <fmr/nowide/filesystem.h>
#include <fmr/nowide/fstream.h>

#include <memory>
#include <vector>

namespace fmr {

namespace file_handler {

namespace filesystem {

using StreamBase = file_handler::local::ReadStream;
class ReadStream : public StreamBase {
  nwd::fs::path path_;
  std::string filename_;
  std::shared_ptr<nwd::ifstream> file_stream_;
  bool loaded_ = false;
  MemoryStream stream_;

 public:
  ReadStream(const ReadStream &stream) = default;
  ReadStream(ReadStream &&stream) = default;

  ReadStream(const nwd::fs::path &path, bool load = false);

  ReadStream(const std::string &path) : ReadStream(Path::MakePath(path)) {}
  ReadStream(const char *path) : ReadStream(Path::MakePath(path)) {}

  std::string GetName() const override { return filename_; }
  const std::string &GetString() override { return filename_; }
  std::string GetHandlerPath() const override {
    return Path::MakeString(path_.parent_path());
  }

  std::string GetFullPath() const override { return Path::MakeString(path_); }

  bool IsShared() const override { return stream_.IsShared(); }

  bool IsDirectory() const override { return Path::IsDirectory(path_); }

  size_t Size() const override;
  size_t FileSize() const;

  const void *GetBuffer() const override { return stream_.GetBuffer(); }

  bool IsLoaded() const override { return loaded_; };
  bool Load() override;

  std::unique_ptr<Stream> Clone() { return std::unique_ptr<Stream>(DoClone()); }

 protected:
  ReadStream *DoClone() const override { return new ReadStream(*this); }
};

}  // namespace filesystem

}  // namespace file_handler

}  // namespace fmr

#endif /* end of include guard: FMR_FILE_HANDLER_FILESYSTEM_STREAM */
