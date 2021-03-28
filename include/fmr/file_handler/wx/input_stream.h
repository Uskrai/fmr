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

#include <cstring>

namespace fmr {

namespace file_handler {

namespace wx {

class InputStream : public wxInputStream {
  const Stream *stream_ = nullptr;
  std::unique_ptr<wxMemoryInputStream> mem_stream_;

  // only used when using constructor with pointer
  std::unique_ptr<Stream> stream_holder_;

 public:
  InputStream(const Stream &stream) : stream_(&stream) { Init(); }

  wxFileOffset GetLength() const override { return mem_stream_->GetLength(); }
  virtual size_t GetSize() const override { return mem_stream_->GetSize(); }
  bool IsSeekable() const override { return mem_stream_->IsSeekable(); }

 protected:
  void Init() {
    const void *buffer = nullptr;
    size_t size = 0;
    if (stream_) {
      buffer = stream_->GetBuffer();
      size = stream_->Size();
    }
    mem_stream_ = std::make_unique<wxMemoryInputStream>(buffer, size);
  }

  size_t OnSysRead(void *buffer_void, size_t size) override {
    mem_stream_->Read(buffer_void, size);
    m_lasterror = mem_stream_->GetLastError();
    return mem_stream_->LastRead();
  }

  wxFileOffset OnSysSeek(wxFileOffset pos, wxSeekMode mode) override {
    return mem_stream_->SeekI(pos, mode);
  }

  wxFileOffset OnSysTell() const override { return mem_stream_->TellI(); }
};

}  // namespace wx

}  // namespace file_handler

}  // namespace fmr

#endif /* end of include guard: H_FMR_FILE_HANDLER_WX_INPUT_STREAM_H */
