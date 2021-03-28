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

#include "fmr/file_handler/wx_archive/input.h"

#include "fmr/file_handler/wx/input_stream.h"
#include "fmr/file_handler/wx_archive/handler.h"
#include "fmr/nowide/string.h"
#include "wx/mstream.h"
#include "wx/wfstream.h"

namespace fmr {

namespace file_handler {

namespace wx_archive {

void Input::Open(std::weak_ptr<file_handler::ReadStream> ptrstream) {
  stream_ = ptrstream;
  if (auto stream = stream_.lock()) factory_ = Handler::FindFactory(*stream);
}

bool Input::IsOpened() const { return factory_; }

ReadStream *Input::GetFirst(bool load_buffer) {
  if (!factory_) return nullptr;

  if (auto stream = stream_.lock()) {
    if (!stream->IsLoaded()) stream->Load();

    auto mem_stream = std::make_unique<wx::InputStream>(*stream);
    arch_input_stream_ = std::unique_ptr<wxArchiveInputStream>(
        factory_->NewStream(mem_stream.release()));
    arch_memory_stream_ = stream;
  }
  return GetNext(load_buffer);
}

ReadStream *Input::GetNext(bool load_buffer) {
  auto archive_stream = stream_.lock();
  if (!arch_input_stream_ && !archive_stream) return nullptr;

  auto entry =
      std::unique_ptr<wxArchiveEntry>(arch_input_stream_->GetNextEntry());

  if (!entry) return nullptr;

  auto &stream = VectorEmplaceBack(archive_stream, entry.get(),
                                   handler_->GetPath(), factory_);

  if (load_buffer) stream.Load(*arch_input_stream_, entry.get());

  return &stream;
}

size_t Input::Index(const std::string &path) const {
  size_t idx = 0;
  for (const auto &it : GetVector()) {
    if (it.GetName() == path) return idx;

    ++idx;
  }
  return -1;
}

void Input::UpdateStream(std::weak_ptr<file_handler::ReadStream> stream) {
  stream_ = stream;
}

void Input::Clear() { ClearVector(); }

// void Input::GetChild(std::vector<const ReadStream *> &vec) const {
// for (const auto &it : vec_) vec.push_back(&it);
// }
//
// void Input::GetChild(std::vector<ReadStream *> &vec) {
// for (auto &it : vec_) vec.push_back(&it);
// }
//
// void Input::GetChild(std::vector<const ReadStreamBase *> &vec) const {
// InputGetChildHelper<const Input, const ReadStream, const
// ReadStreamBase>(this, vec);
// }
//
// void Input::GetChild(std::vector<ReadStreamBase *> &vec) {
// InputGetChildHelper<Input, ReadStream, ReadStreamBase>(this, vec);
// }

}  // namespace wx_archive

}  // namespace file_handler

}  // namespace fmr
