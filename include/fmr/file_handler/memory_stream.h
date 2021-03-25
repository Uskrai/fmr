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
  MemoryStream(const file_handler::Stream &stream);
  const std::string &GetString() const override { return name_; }
  std::string GetName() const override { return name_; }
  std::string GetHandlerPath() const override { return ""; }

  void MakeExclusive();

  void Resize(size_t size);

  size_t Size() const override { return buffer_->size(); }

  const void *GetBuffer() const override { return buffer_->data(); }
  const void *GetBufferEnd() const override { return buffer_->data() + Size(); }

  bool IsShared() const override { return buffer_.use_count() > 1; }
  bool IsDirectory() const override { return false; }

  bool IsEmpty() const { return Size() == 0; }

  void Write(const void *src, size_t size) override;
  // void Write(const Stream &src) override;
  void Write(const file_handler::Stream &src) override;
  void Write(std::shared_ptr<Bytes> vec);

  void Clear() { buffer_ = nullptr; }

  std::unique_ptr<MemoryStream> Clone() const {
    return std::unique_ptr<MemoryStream>(DoClone());
  }

 protected:
  MemoryStream *DoClone() const override { return new MemoryStream(*this); }

  void DoWrite(const file_handler::Stream &src);
  void DoWrite(const void *src, size_t size);
  void DoWrite(std::shared_ptr<Bytes> vec);
};

}  // namespace file_handler

}  // namespace fmr

#endif /* end of include guard: FMR_FILE_HANDLER_MEMORY_STREAM */
