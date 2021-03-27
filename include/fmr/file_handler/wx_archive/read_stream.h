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

#ifndef FMR_FILE_HANDLER_WX_ARCHIVE_READ_STREAM
#define FMR_FILE_HANDLER_WX_ARCHIVE_READ_STREAM

#include <fmr/file_handler/read_stream.h>
#include <wx/archive.h>

#include "fmr/file_handler/utility/memory_stream_helper.h"

namespace fmr {

namespace file_handler {

namespace wx_archive {

using ReadStreamBase =
    utility::ReadMemoryStreamHelper<file_handler::ReadStream>;
class ReadStream : public ReadStreamBase {
  std::shared_ptr<file_handler::ReadStream> archive_stream_;
  const wxArchiveClassFactory *factory_ = nullptr;
  bool loaded_ = false;
  bool is_directory_ = false;

 public:
  ReadStream(std::shared_ptr<file_handler::ReadStream> archive_stream,
             wxArchiveEntry *entry, std::string handler_path_,
             const wxArchiveClassFactory *factory);

  bool IsDirectory() const override { return is_directory_; }

  bool IsLoaded() const override { return loaded_; };
  bool Load() override;
  bool Load(wxArchiveInputStream &stream, wxArchiveEntry *entry);

  std::unique_ptr<ReadStream> Clone() const {
    return std::unique_ptr<ReadStream>(DoClone());
  }

 protected:
  virtual ReadStream *DoClone() const override { return new ReadStream(*this); }
};

}  // namespace wx_archive

}  // namespace file_handler

}  // namespace fmr

#endif /* end of include guard: FMR_FILE_HANDLER_WX_ARCHIVE_READ_STREAM */
