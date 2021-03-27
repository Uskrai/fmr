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

#ifndef H_FMR_FILE_HANDLER_VIRTUAL_WRITE_STREAM_H
#define H_FMR_FILE_HANDLER_VIRTUAL_WRITE_STREAM_H

#include <fmr/file_handler/read_stream.h>
#include <fmr/file_handler/write_stream.h>

namespace fmr {

namespace file_handler {

namespace virtual_write {

class Stream : public compare::Sortable {
 public:
  enum StreamType {
    kDefaultStream = 0x00,
    kReadStream = 0x01,
    kWriteStream = 0x02
  };

 private:
  StreamType s_type_ = kDefaultStream;
  WriteType w_type_ = kWriteNone;
  std::string string_;

  bool is_dir_ = false;

  const file_handler::Stream *stream_ = nullptr;

 public:
  Stream() : Stream(nullptr, kDefaultStream, kWriteNone) {}
  Stream(const file_handler::Stream *stream, WriteType type)
      : Stream(stream, kDefaultStream, type) {}

  Stream(const file_handler::ReadStream *stream, WriteType type)
      : Stream(stream, kReadStream, type) {}

  Stream(const file_handler::WriteStream *stream, WriteType type)
      : Stream(stream, kWriteStream, type) {}

  std::string GetName() const {
    return GetStream() ? GetStream()->GetName() : "";
  }

  const std::string &GetString() const override { return string_; }

  bool IsDirectory() const { return is_dir_; }

  StreamType GetStreamType() const { return s_type_; }
  bool IsReadStream() const { return GetStreamType() & kReadStream; }
  bool IsWriteStream() const { return GetStreamType() & kWriteStream; }

  WriteType GetWriteType() const { return w_type_; }

  const file_handler::Stream *GetStream() const { return stream_; }

  const file_handler::ReadStream *GetAsReadStream() const {
    return IsReadStream()
               ? static_cast<const file_handler::ReadStream *>(GetStream())
               : nullptr;
  }

  const file_handler::WriteStream *GetAsWriteStream() const {
    return IsWriteStream()
               ? static_cast<const file_handler::WriteStream *>(GetStream())
               : nullptr;
  }

 private:
  Stream(const file_handler::Stream *stream, StreamType s_type,
         WriteType w_type)
      : s_type_(s_type),
        w_type_(w_type),
        string_(stream ? stream->GetString() : ""),
        is_dir_(stream && stream->IsDirectory()),
        stream_(stream) {}
};

}  // namespace virtual_write

}  // namespace file_handler

}  // namespace fmr

#endif /* end of include guard: H_FMR_FILE_HANDLER_VIRTUAL_WRITE_STREAM_H */
