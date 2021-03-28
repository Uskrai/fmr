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

#ifndef H_FMR_FILE_HANDLER_WX_OUTPUT_STREAM_H
#define H_FMR_FILE_HANDLER_WX_OUTPUT_STREAM_H

#include <fmr/file_handler/write_stream.h>
#include <wx/stream.h>

namespace fmr {

namespace file_handler {

namespace wx {

class OutputStream : public wxOutputStream {
  WriteStream *stream_ = nullptr;
  std::unique_ptr<WriteStream> ptr_holder_;

 public:
  OutputStream(WriteStream &stream) : stream_(&stream){};
  virtual ~OutputStream(){};

 protected:
  size_t OnSysWrite(const void *buffer, size_t bufsize) {
    auto last_size = stream_->Size();
    stream_->Write(buffer, bufsize);
    return stream_->Size() - last_size;
  }
};

}  // namespace wx

}  // namespace file_handler

}  // namespace fmr

#endif /* end of include guard: H_FMR_FILE_HANDLER_WX_OUTPUT_STREAM_H */
