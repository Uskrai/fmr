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

#ifndef FMR_FILE_HANDLER_UTILITY_MEMORY_STREAM_HELPER
#define FMR_FILE_HANDLER_UTILITY_MEMORY_STREAM_HELPER

#include <fmr/file_handler/memory_stream.h>
#include <fmr/file_handler/stream.h>

#include <exception>

namespace fmr {

namespace file_handler {

namespace utility {

namespace internal {

namespace memory_stream {

class HelperBase {
  MemoryStream stream_;
  std::string handler_path_, name_;
  WriteType type_ = kWriteNone;

 public:
  HelperBase() {}
  HelperBase(const file_handler::Stream &stream) : stream_(stream) {
    DoCopyStreamContent(stream);
  }
  HelperBase(const MemoryStream &stream) : stream_(stream) {
    DoCopyStreamContent(stream);
  }

 protected:
  void DoSetName(const std::string &name) { name_ = name; }
  void DoSetHandlerPath(const std::string &path) { handler_path_ = path; }

  const std::string &DoGetName() const { return name_; }
  const std::string &DoGetHandlerPath() const { return handler_path_; }

  const void *DoGetBuffer() const { return stream_.GetBuffer(); }
  const void *DoGetBufferEnd() const { return stream_.GetBufferEnd(); }
  size_t DoGetSize() const { return stream_.Size(); }

  bool DoIsShared() const { return stream_.IsShared(); }
  bool DoIsDirectory() const { return stream_.IsDirectory(); }

  void DoWrite(const void *buffer, size_t size) {
    return stream_.Write(buffer, size);
  }

  void DoWrite(const file_handler::Stream &stream) {
    return stream_.Write(stream);
  }

  void DoWrite(std::shared_ptr<std::vector<std::byte>> vec) {
    return stream_.Write(vec);
  }

  void DoStreamClear() { return stream_.Clear(); }

  WriteType DoGetWriteType() const { return type_; }
  void DoSetWriteType(WriteType type) { type_ = type; }

  void DoCopyStreamContent(const file_handler::Stream &stream) {
    DoSetName(stream.GetName());
    DoSetHandlerPath(stream.GetHandlerPath());
  }
};

template <typename StreamBase>
class Helper : public StreamBase, public HelperBase {
 public:
  using HelperBase::HelperBase;
  Helper(const std::string &handler_path, const std::string &name,
         WriteType type = kWriteNone) {
    DoSetHandlerPath(handler_path);
    DoSetName(name);
    DoSetWriteType(type);
  };

  using Bytes = std::vector<std::byte>;

  const std::string &GetString() const { return DoGetName(); }
  std::string GetName() const { return DoGetName(); }
  std::string GetHandlerPath() const { return DoGetHandlerPath(); }

  const void *GetBuffer() const { return DoGetBuffer(); }
  const void *GetBufferEnd() const { return DoGetBufferEnd(); }

  size_t Size() const { return DoGetSize(); }

  bool IsShared() const { return DoIsShared(); }
};

template <typename Base>
class ClonableStream : public Base {
 public:
  ClonableStream<Base> *DoClone() const { return nullptr; }
};

namespace write {

template <typename WriteStreamBase>
class HelperBase : public Helper<WriteStreamBase> {
 public:
  using BaseClass = Helper<WriteStreamBase>;
  using BaseClass::BaseClass;
  void Write(const void *buffer, size_t size) {
    HelperBase::DoWrite(buffer, size);
  }
  void Write(const file_handler::Stream &stream) {
    HelperBase::DoWrite(stream);
  }

  WriteType GetWriteType() const { return HelperBase::DoGetWriteType(); }
};

template <typename T, typename U = std::void_t<T>>
class Helper {};

template <typename WriteStreamBase>
class Helper<WriteStreamBase,
             std::enable_if_t<std::is_abstract_v<HelperBase<WriteStreamBase>>,
                              std::void_t<WriteStreamBase>>>
    : public HelperBase<WriteStreamBase> {
  using CurrentClass = Helper<WriteStreamBase>;

 public:
  using HelperBase<WriteStreamBase>::HelperBase;

 public:
  // std::unique_ptr<CurrentClass> Clone() const {
  // return std::unique_ptr<CurrentClass>(DoClone());
  // }

  // protected:
  // virtual CurrentClass *DoClone() const = 0;
};

template <typename WriteStreamBase>
class Helper<WriteStreamBase,
             std::enable_if_t<!std::is_abstract_v<HelperBase<WriteStreamBase>>,
                              std::void_t<WriteStreamBase>>>
    : public HelperBase<WriteStreamBase> {
  using CurrentClass = Helper<WriteStreamBase>;

 public:
  using HelperBase<WriteStreamBase>::HelperBase;

  std::unique_ptr<CurrentClass> Clone() const {
    return std::unique_ptr<CurrentClass>(DoClone());
  }

 protected:
  CurrentClass *DoClone() const { return new CurrentClass(*this); }
};

}  // namespace write

namespace read {

template <typename ReadStreamBase>
class HelperBase : public memory_stream::Helper<ReadStreamBase> {
 public:
  using memory_stream::Helper<ReadStreamBase>::Helper;
  bool IsLoaded() const { return true; }
  bool Load() { return true; }

  bool IsDirectory() const {
    return memory_stream::Helper<ReadStreamBase>::DoIsDirectory();
  }
};

template <typename ReadStreamBase, typename = void>
class Helper : public HelperBase<ReadStreamBase> {
 public:
  using HelperBase<ReadStreamBase>::HelperBase;
};

template <typename ReadStreamBase>
class Helper<ReadStreamBase,
             std::enable_if_t<!std::is_abstract_v<
                                  ClonableStream<HelperBase<ReadStreamBase>>>,
                              std::void_t<ReadStreamBase>>>
    : public HelperBase<ReadStreamBase> {
 public:
  using HelperBase<ReadStreamBase>::HelperBase;

  std::unique_ptr<Helper<ReadStreamBase>> Clone() const {
    return std::unique_ptr<Helper<ReadStreamBase>>(DoClone());
  }

 protected:
  Helper<ReadStreamBase> *DoClone() const {
    return new Helper<ReadStreamBase>(*this);
  }
};

}  // namespace read

}  // namespace memory_stream

}  // namespace internal

template <typename WriteStreamBase>
using WriteMemoryStreamHelper =
    internal::memory_stream::write::Helper<WriteStreamBase>;

template <typename ReadStreamBase>
using ReadMemoryStreamHelper =
    internal::memory_stream::read::Helper<ReadStreamBase>;

// template <typename WriteStreamBase>
// class MemoryStreamTemplate : public WriteStreamBase {
// using ClassType = MemoryStreamTemplate<WriteStreamBase>;
//
// MemoryStream buffer_;
//
// std::string name_, handler_path_;
//
// public:
// MemoryStreamTemplate(const std::string &handler_path, const std::string
// &name, WriteType type)
// : WriteStreamBase(type), name_(name), handler_path_(handler_path){};
// MemoryStreamTemplate(const std::string &handler_path, const std::string
// &name, WriteType type, file_handler::Stream &stream)
// : MemoryStreamTemplate(handler_path, name, type) {
// buffer_.Write(stream);
// };
// MemoryStreamTemplate(const ClassType &oth) = default;
// MemoryStreamTemplate(ClassType &&oth) = default;
//
// ClassType &operator=(const ClassType &oth) = default;
// ClassType &operator=(ClassType &&oth) = default;
//
// template <typename OtherType>
// MemoryStreamTemplate(const MemoryStreamTemplate<OtherType> &oth)
// : buffer_(oth.buffer_),
// name_(oth.name_),
// handler_path_(oth.handler_path_) {}
//
// const std::string &GetString() const override { return name_; }
// std::string GetName() const override { return name_; }
// std::string GetHandlerPath() const override { return handler_path_; }
//
// size_t Size() const override { return buffer_.Size(); }
//
// const void *GetBuffer() const override { return buffer_.GetBuffer(); }
// const void *GetBufferEnd() const override { return buffer_.GetBufferEnd(); }
//
// bool IsShared() const override { return buffer_.IsShared(); }
// bool IsDirectory() const override {
// return WriteStreamBase::GetWriteType() & kWriteDirectory;
// }
//
// void Write(const void *src, size_t size) override {
// buffer_.Write(src, size);
// }
// void Write(const Stream &src) override { buffer_.Write(src); }
//
// std::unique_ptr<ClassType> Clone() const {
// return std::unique_ptr<ClassType>(DoClone());
// }
//
// protected:
// ClassType *DoClone() const override = 0;
//
// void SetHandlerPath(const std::string &path) { handler_path_ = path; }
// void SetName(const std::string &name) { name_ = name; }
// };

}  // namespace utility

}  // namespace file_handler

}  // namespace fmr

#endif /* end of include guard: FMR_FILE_HANDLER_UTILITY_MEMORY_STREAM_HELPER \
        */
