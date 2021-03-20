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

#ifndef FMR_FILE_HANDLER_OUTPUT_STREAM
#define FMR_FILE_HANDLER_OUTPUT_STREAM

#include <fmr/file_handler/stream.h>
#include <fmr/file_handler/write_type.h>

namespace fmr {

namespace file_handler {

class WriteStream : public Stream {
  WriteType type_;

 public:
  WriteStream() : type_(kWriteNone) {}
  WriteStream(WriteType type) : type_(type) {}

  virtual void Write(const void *src, size_t size) = 0;
  void Write(const Stream *src) = delete;
  virtual void Write(const Stream &src) = 0;

  virtual void SetWriteType(WriteType type) { type_ = type; }
  virtual WriteType GetWriteType() const { return type_; }

 protected:
  virtual WriteStream *DoClone() const override = 0;
};

}  // namespace file_handler

}  // namespace fmr

#endif /* end of include guard: FMR_FILE_HANDLER_OUTPUT_STREAM */
