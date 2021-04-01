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

#include "fmr/file_handler/wx/input_stream.h"

#include <cstring>

namespace fmr {

namespace file_handler {

namespace wx {

namespace WIP {

size_t InputStream::OnSysRead(void *buffer, size_t size) {
  memset(buffer, 0x00, size);

  size_t pos = GetIntPosition(), length = GetLength();
  if (pos == length) {
    m_lasterror = wxSTREAM_EOF;
    printf("EOF\n");
    return 0;
  }

  size_t read_bytes = ReadFromBuffer(buffer, size);
  m_lasterror = wxSTREAM_NO_ERROR;

  return read_bytes;
}

size_t InputStream::ReadFromBuffer(void *buffer, size_t size) {
  size_t orig_size = size;
  if (size > GetSize()) size = GetSize();

  while (size > 0) {
    size_t left = GetDataLeft();

    if (size > left) {
      size -= left;
      GetFromBuffer(buffer, size);
      break;
    } else {
      GetFromBuffer(buffer, size);
      size = 0;
    }
  }

  size_t read_bytes = orig_size - size;

  return read_bytes;
}

void InputStream::GetFromBuffer(void *buffer, size_t size) {
  size_t left = GetBytesLeft();
  if (size > left) size = left;
  std::memcpy(buffer, buffer_pos_, size);
  buffer_pos_ += size;
}

wxFileOffset InputStream::OnSysSeek(wxFileOffset pos, wxSeekMode mode) {
  wxFileOffset diff, last_access = GetLastAccess();

  switch (mode) {
    case wxFromStart:
      diff = pos;
      break;
    case wxFromCurrent:
      diff = pos + GetIntPosition();
      break;
    case wxFromEnd:
      diff = pos + last_access;
      break;
    default:
      printf("Invalid mode\n");
      return wxInvalidOffset;
  }

  if (diff < 0 || diff > last_access) return wxInvalidOffset;
  printf("%ld\n", diff);
  SetIntPosition(diff);
  return diff;
}

wxFileOffset InputStream::OnSysTell() const { return GetIntPosition(); }

}  // namespace WIP

}  // namespace wx

}  // namespace file_handler

}  // namespace fmr
