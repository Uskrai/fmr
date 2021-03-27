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

#include "fmr/file_handler/wx_archive/output.h"

#include <algorithm>

#include "fmr/common/compare.h"
#include "fmr/compare/comparer.h"
#include "fmr/compare/natural.h"
#include "fmr/compare/sortable.h"
#include "fmr/file_handler/utility/memory_stream_helper.h"
#include "fmr/file_handler/virtual_write/map.h"
#include "fmr/file_handler/write_type.h"
#include "fmr/file_handler/wx_archive/handler.h"
#include "fmr/file_handler/wx_archive/input.h"
#include "fmr/nowide/string.h"
#include "wx/archive.h"
#include "wx/filefn.h"
#include "wx/mstream.h"
#include "wx/wfstream.h"

namespace fmr {

namespace file_handler {

namespace wx_archive {

class wxToFmrOutputStream : public wxOutputStream {
  file_handler::WriteStream *stream_;

 public:
  wxToFmrOutputStream(file_handler::WriteStream *stream) : stream_(stream) {}

 protected:
  size_t OnSysWrite(const void *buffer, size_t size) override {
    stream_->Write(buffer, size);
    return size;
  }
};

class wxToFmrInputStream : public wxInputStream {
  file_handler::Stream *stream_;

 public:
  wxToFmrInputStream(file_handler::Stream *stream) : stream_(stream) {}

 protected:
  size_t OnSysRead(void *buffer, size_t size) override {
    auto dst = static_cast<std::byte *>(buffer);
    auto src = static_cast<const std::byte *>(stream_->GetBuffer());
    auto src_end = static_cast<const std::byte *>(stream_->GetBuffer()) + size;
    auto ret = std::copy(src, src_end, dst);
    return ret - dst;
  }
};

Output::Output(Handler *handler, Input *input) {
  handler_ = handler;
  input_ = input;
}

void Output::Open(std::shared_ptr<file_handler::ReadStream> stream,
                  const wxArchiveClassFactory *arch_factory,
                  file_handler::Handler *handler_parent) {
  stream_ = stream;
  archive_factory_ = arch_factory;
  handler_parent_ = handler_parent;
}

bool Output::IsOk() const {
  return stream_ && stream_->GetName() != "" && archive_factory_ && handler_ &&
         handler_parent_ && handler_parent_->IsExist();
}

std::vector<virtual_write::Stream> Output::ToVirtualStream(Input &input) const {
  input.Open(stream_);
  input.Traverse(true);

  auto map_path = virtual_write::Map('/');
  for (auto &it : input) map_path.AddEmplace(&it, kWriteNone);
  for (auto &it : vec_) {
    map_path.AddEmplace(&it, it.GetWriteType());
  }

  return map_path.ToVector(map_path.kExtractAll);
}

void Output::CommitWrite() {
  if (!IsOk()) return;

  auto write_stream = handler_parent_->Write()->CreateFile(
      stream_->GetName(), nullptr, kWriteOverwrite);

  auto in_stream = wxMemoryInputStream(stream_->GetBuffer(), stream_->Size());
  auto archive_input = std::unique_ptr<wxArchiveInputStream>(
      archive_factory_->NewStream(in_stream));

  wxToFmrOutputStream output(write_stream);

  auto archive_output = std::unique_ptr<wxArchiveOutputStream>(
      archive_factory_->NewStream(output));

  if (archive_input->IsOk() && archive_input->GetSize() != 0)
    archive_output->CopyArchiveMetaData(*archive_input);

  Input input(handler_);
  auto vec = ToVirtualStream(input);

  compare::Natural comparer;
  compare::Sort(vec.begin(), vec.end(), comparer);

  for (auto it = vec.begin(); it != vec.end(); ++it) {
    auto stream = it->GetStream();

    if (!stream) continue;

    if (it->IsDirectory()) {
      archive_output->PutNextDirEntry(it->GetName());
    } else {
      archive_output->PutNextEntry(it->GetName());
      archive_output->Write(stream->GetBuffer(), stream->Size());
    }
  }

  archive_input.reset();

  if (archive_output->Close()) {
    UpdateStream(*write_stream);
    handler_parent_->Write()->CommitWrite();
  }
}

void Output::Create() {
  if (!handler_parent_) return;

  auto write = handler_parent_->Write()->CreateFile(stream_->GetName(), nullptr,
                                                    kWriteNone);
  auto wx_output = wxToFmrOutputStream(write);

  auto archive_output = std::unique_ptr<wxArchiveOutputStream>(
      archive_factory_->NewStream(wx_output));

  if (archive_output->Close()) {
    UpdateStream(*write);
    handler_parent_->Write()->CommitWrite();
  }
}

void Output::CreateDirectory(const std::string &name) {
  CreateFile(name, nullptr, kWriteDirectory);
}

void Output::UpdateStream(const Stream &stream) {
  stream_ = std::make_shared<
      utility::ReadMemoryStreamHelper<file_handler::ReadStream>>(stream);
  handler_->UpdateStream(stream_);
}

WriteStream *Output::CreateFile(const std::string &name, Stream *stream,
                                WriteType type) {
  auto &ret_stream = vec_.emplace_back(handler_->GetPath(), name, type);
  if (stream) ret_stream.Write(*stream);
  return &ret_stream;
}

void Output::DeleteDirectory(const std::string &name) {}

void Output::DeleteFile(const std::string &name) {}

void Output::Delete() {
  if (!IsOk()) return;

  handler_parent_->Write()->DeleteFile(stream_->GetName());
  handler_parent_->Write()->CommitWrite();
}

}  // namespace wx_archive

}  // namespace file_handler

}  // namespace fmr
