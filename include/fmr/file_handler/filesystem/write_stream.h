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

#include <fmr/common/path.h>
#include <fmr/file_handler/local/write_stream.h>
#include <fmr/file_handler/utility/memory_stream_helper.h>
#include <fmr/file_handler/write_type.h>

namespace fmr {

namespace file_handler {

namespace filesystem {

using WriteStreamBase = utility::WriteMemoryStreamHelper<local::WriteStream>;

class WriteStream : public WriteStreamBase {
 public:
  using WriteStreamBase::WriteStreamBase;

  std::string GetFullPath() const override {
    return Path::Append(GetHandlerPath(), GetName());
  }

 protected:
  WriteStream *DoClone() const override { return new WriteStream(*this); }
};
// class WriteStream : public WriteStreamBase {
// std::string name_, handler_path_;
// WriteType type_;
// MemoryStream stream_;
// bool is_dir_ = false;
//
// public:
// WriteStream(const std::string &handler, const std::string &name,
// WriteType type)
// : WriteStreamBase(type), name_(name), handler_path_(handler){};
//
// const std::string &GetString() override { return name_; }
// std::string GetName() const override { return name_; }
//
// std::string GetFullPath() const override {
// return Path::Append(GetHandlerPath(), GetName());
// }
//
// std::string GetHandlerPath() const override { return handler_path_; }
//
// const void *GetBuffer() const override { return stream_.GetBuffer(); }
// size_t Size() const override { return stream_.Size(); }
//
// bool IsShared() const override { return stream_.IsShared(); }
// bool IsDirectory() const override { return is_dir_; }
//
// void Write(const void *src, size_t size) override {
// return stream_.Write(src, size);
// }
// void Write(const Stream &src) override { return stream_.Write(src); }
//
// std::unique_ptr<WriteStream> Clone() const {
// return std::unique_ptr<WriteStream>(DoClone());
// }
//
// protected:
// WriteStream *DoClone() const override { return new WriteStream(*this); }
// };

}  // namespace filesystem

}  // namespace file_handler

}  // namespace fmr
