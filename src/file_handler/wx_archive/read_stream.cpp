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

#include "fmr/file_handler/wx_archive/read_stream.h"

#include "fmr/file_handler/wx_archive/input.h"
#include "fmr/nowide/string.h"
#include "wx/mstream.h"

namespace fmr {

namespace file_handler {

namespace wx_archive {

ReadStream::ReadStream(std::shared_ptr<file_handler::ReadStream> archive_stream,
                       wxArchiveEntry *entry, std::string handler_path,
                       const wxArchiveClassFactory *factory) {
  archive_stream_ = archive_stream;
  is_directory_ = entry->IsDir();
  DoSetName(String::Narrow(entry->GetName()));
  DoSetHandlerPath(handler_path);
  factory_ = factory;
}

bool ReadStream::Load() {
  if (archive_stream_) {
    wxMemoryInputStream mem_stream(archive_stream_->GetBuffer(),
                                   archive_stream_->Size());

    auto arch_stream =
        std::unique_ptr<wxArchiveInputStream>(factory_->NewStream(mem_stream));

    std::unique_ptr<wxArchiveEntry> entry;
    wxString name = String::Widen<wxString>(GetName());
    do {
      entry.reset(arch_stream->GetNextEntry());
    } while (entry && entry->GetName() != name);

    return entry && Load(*arch_stream, entry.get());
  }

  return false;
}

bool ReadStream::Load(wxArchiveInputStream &stream, wxArchiveEntry *entry) {
  if (entry && entry->GetName() == String::Widen<wxString>(GetName())) {
    auto vec = std::make_shared<Bytes>();
    vec->resize(stream.GetSize());

    stream.Read(vec->data(), stream.GetSize()).LastRead();

    DoWrite(vec);
    loaded_ = true;
  }
  return loaded_;
}

}  // namespace wx_archive

}  // namespace file_handler

}  // namespace fmr
