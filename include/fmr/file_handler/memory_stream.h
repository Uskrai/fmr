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

#ifndef FMR_FILE_HANDLER_MEMORY_STREAM
#define FMR_FILE_HANDLER_MEMORY_STREAM

#include <fmr/file_handler/write_stream.h>

#include <memory>
#include <vector>

namespace fmr {

namespace file_handler {

class MemoryStream : public WriteStream {
  typedef std::vector<std::byte> Bytes;
  std::shared_ptr<Bytes> buffer_;
  std::string name_;

 public:
  MemoryStream();
  MemoryStream(const file_handler::Stream &stream) : MemoryStream() {
    Write(stream);
  }

  const std::string &GetString() override { return name_; }
  std::string GetName() const override { return name_; }
  std::string GetHandlerPath() const override { return ""; }

  void MakeExclusive();

  void Resize(size_t size);

  size_t Size() const override { return buffer_->size(); }

  const void *GetBuffer() const override { return buffer_->data(); }

  bool IsShared() const override { return buffer_.use_count() > 1; }
  bool IsDirectory() const override { return false; }

  void Write(const void *src, size_t size) override;
  void Write(const Stream &src) override;
  void Write(std::shared_ptr<Bytes> vec) { buffer_ = vec; }

  void Clear() { buffer_ = nullptr; }

  std::unique_ptr<MemoryStream> Clone() const {
    return std::unique_ptr<MemoryStream>(DoClone());
  }

 protected:
  MemoryStream *DoClone() const override { return new MemoryStream(*this); }
};

template <typename WriteStreamBase>
class MemoryStreamTemplate : public WriteStreamBase {
  using ClassType = MemoryStreamTemplate<WriteStreamBase>;

  MemoryStream buffer_;
  std::string name_, handler_path_;

 public:
  MemoryStreamTemplate(const std::string &handler_path, const std::string &name,
                       WriteType type)
      : WriteStreamBase(type), name_(name), handler_path_(handler_path){};
  MemoryStreamTemplate(const std::string &handler_path, const std::string &name,
                       WriteType type, file_handler::Stream &stream)
      : MemoryStreamTemplate(handler_path, name, type) {
    buffer_.Write(stream);
  };
  MemoryStreamTemplate(const ClassType &oth) = default;
  MemoryStreamTemplate(ClassType &&oth) = default;

  template <typename OtherType>
  MemoryStreamTemplate(const MemoryStreamTemplate<OtherType> &oth)
      : buffer_(oth.buffer_),
        name_(oth.name_),
        handler_path_(oth.handler_path_) {}

  const std::string &GetString() override { return name_; }
  std::string GetName() const override { return name_; }
  std::string GetHandlerPath() const override { return handler_path_; }

  size_t Size() const override { return buffer_.Size(); }

  const void *GetBuffer() const override { return buffer_.GetBuffer(); }

  bool IsShared() const override { return buffer_.IsShared(); }
  bool IsDirectory() const override { return false; }

  void Write(const void *src, size_t size) override {
    buffer_.Write(src, size);
  }
  void Write(const Stream &src) override { buffer_.Write(src); }

  std::unique_ptr<ClassType> Clone() const {
    return std::unique_ptr<ClassType>(DoClone());
  }

 protected:
  ClassType *DoClone() const override = 0;

  void SetHandlerPath(const std::string &path) { handler_path_ = path; }
  void SetName(const std::string &name) { name_ = name; }
};

}  // namespace file_handler

}  // namespace fmr

#endif /* end of include guard: FMR_FILE_HANDLER_MEMORY_STREAM */
