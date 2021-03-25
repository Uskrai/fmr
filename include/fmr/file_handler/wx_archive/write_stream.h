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

#ifndef FMR_FILE_HANDLER_WX_ARCHIVE_WRITE_STREAM
#define FMR_FILE_HANDLER_WX_ARCHIVE_WRITE_STREAM

#include <fmr/file_handler/utility/memory_stream_helper.h>
#include <fmr/file_handler/write_stream.h>

namespace fmr {

namespace file_handler {

namespace wx_archive {

using WriteStreamBase =
    utility::WriteMemoryStreamHelper<file_handler::WriteStream>;
class WriteStream : public WriteStreamBase {
 public:
  using WriteStreamBase::WriteStreamBase;

  std::unique_ptr<WriteStream> Clone() const {
    return std::unique_ptr<WriteStream>(DoClone());
  }

 protected:
  WriteStream *DoClone() const override { return new WriteStream(*this); }
};

}  // namespace wx_archive

}  // namespace file_handler

}  // namespace fmr

#endif /* end of include guard: FMR_FILE_HANDLER_WX_ARCHIVE_WRITE_STREAM */
