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

#ifndef H_FMR_FILE_HANDLER_WX_INPUT_STREAM_H
#define H_FMR_FILE_HANDLER_WX_INPUT_STREAM_H

#include <fmr/file_handler/stream.h>
#include <wx/mstream.h>
#include <wx/stream.h>

namespace fmr {

namespace file_handler {

namespace wx {

class InputStream : public wxInputStream {
  const Stream *stream_ = nullptr;
  wxMemoryInputStream mem_stream_;

 public:
  InputStream(const Stream &stream)
      : stream_(&stream), mem_stream_(stream.GetBuffer(), stream.Size()) {}

  wxFileOffset GetLength() const override { return mem_stream_.GetLength(); }

  bool IsSeekable() const override { return mem_stream_.IsSeekable(); }

  const Stream *GetStream() const { return stream_; }

 protected:
  size_t OnSysRead(void *buffer, size_t size) override {
    m_lasterror = mem_stream_.Read(buffer, size).GetLastError();
    return mem_stream_.LastRead();
  }

  wxFileOffset OnSysSeek(wxFileOffset pos, wxSeekMode mode) override {
    return mem_stream_.SeekI(pos, mode);
  }
  wxFileOffset OnSysTell() const override { return mem_stream_.TellI(); }
};

namespace WIP {
class InputStream : public wxInputStream {
  const Stream *stream_ = nullptr;

  // only used when using constructor with pointer
  std::unique_ptr<Stream> stream_holder_;
  const std::byte *buffer_start_, *buffer_pos_, *buffer_end_;
  wxMemoryInputStream mem_stream_;

 public:
  InputStream(const Stream &stream)
      : stream_(&stream), mem_stream_(stream.GetBuffer(), stream.Size()) {
    Init();
  }

  wxFileOffset GetLength() const override {
    return buffer_end_ - buffer_start_;
  }

  bool IsSeekable() const override { return true; }

 protected:
  void Init() {
    buffer_start_ = static_cast<const std::byte *>(stream_->GetBuffer());
    buffer_end_ = static_cast<const std::byte *>(stream_->GetBufferEnd());
    buffer_pos_ = buffer_start_;
  }

  size_t GetIntPosition() const { return buffer_pos_ - buffer_start_; }
  void SetIntPosition(size_t pos) { buffer_pos_ = buffer_start_ + pos; }
  size_t GetLastAccess() const { return buffer_end_ - buffer_start_; }
  size_t GetBytesLeft() const { return buffer_end_ - buffer_pos_; }
  size_t GetDataLeft() { return GetBytesLeft(); }

  size_t OnSysRead(void *buffer, size_t size) override;

  size_t ReadFromBuffer(void *buffer, size_t size);
  void GetFromBuffer(void *buffer, size_t size);

  wxFileOffset OnSysSeek(wxFileOffset pos, wxSeekMode mode) override;
  wxFileOffset OnSysTell() const override;
};

}  // namespace WIP

}  // namespace wx

}  // namespace file_handler

}  // namespace fmr

#endif /* end of include guard: H_FMR_FILE_HANDLER_WX_INPUT_STREAM_H */
