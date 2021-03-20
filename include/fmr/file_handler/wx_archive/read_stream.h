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

#include "fmr/file_handler/memory_stream.h"

namespace fmr {

namespace file_handler {

namespace wx_archive {

class ReadStream : public file_handler::ReadStream {
  std::shared_ptr<file_handler::ReadStream> archive_stream_;
  std::string handler_path_, name_;
  const wxArchiveClassFactory *factory_ = nullptr;
  wxArchiveEntry *entry_ = nullptr;
  bool loaded_ = false;
  MemoryStream stream_;

 public:
  ReadStream(std::shared_ptr<file_handler::ReadStream> archive_stream,
             wxArchiveEntry *entry, std::string handler_path_);

  const std::string &GetString() override { return name_; }
  std::string GetName() const override { return name_; }

  const void *GetBuffer() const override { return stream_.GetBuffer(); }

  size_t Size() const override { return stream_.Size(); }

  bool IsDirectory() const override { return entry_ && entry_->IsDir(); }
  bool IsShared() const override { return stream_.IsShared(); }
  bool IsLoaded() const override;
  bool Load() override;
  bool Load(wxArchiveInputStream &stream, wxArchiveEntry *entry);

  std::string GetHandlerPath() const override { return handler_path_; }

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
