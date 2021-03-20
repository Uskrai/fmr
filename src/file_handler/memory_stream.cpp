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

#include <fmr/file_handler/memory_stream.h>

namespace fmr {

namespace file_handler {

MemoryStream::MemoryStream() { buffer_ = std::make_shared<Bytes>(); }

void MemoryStream::MakeExclusive() {
  if (!buffer_)
    buffer_ = std::make_shared<Bytes>();

  else if (IsShared())
    buffer_ = std::make_shared<Bytes>(*buffer_);
}

void MemoryStream::Resize(size_t size) {
  MakeExclusive();
  buffer_->resize(size);
}

void MemoryStream::Write(const void *src, size_t size) {
  MakeExclusive();
  auto buffer = reinterpret_cast<const std::byte *>(src);

  buffer_->reserve(Size() + size);
  for (size_t i = 0; i < size; ++i) buffer_->push_back(buffer[i]);
}

void MemoryStream::Write(const Stream &src) {
  Write(src.GetBuffer(), src.Size());
}

}  // namespace file_handler

}  // namespace fmr
