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

#include "fmr/file_handler/wx_archive/handler.h"
#include "wx/mstream.h"

namespace fmr {

namespace file_handler {

namespace wx_archive {

void Input::Open(std::weak_ptr<file_handler::ReadStream> ptrstream) {
  stream_ = ptrstream;
  if (auto stream = stream_.lock()) factory_ = Handler::FindFactory(*stream);
}

bool Input::IsEmpty() const { return true; }

bool Input::IsOpened() const { return factory_; }

ReadStream *Input::GetFirst(bool load_buffer) {
  if (!factory_) return nullptr;

  if (auto stream = stream_.lock()) {
    auto mem_stream = std::make_unique<wxMemoryInputStream>(stream->GetBuffer(),
                                                            stream->Size());

    auto arch_stream = std::unique_ptr<wxArchiveInputStream>(
        factory_->NewStream(mem_stream.release()));

    return GetNext(load_buffer);
  }
  return nullptr;
}

ReadStream *Input::GetNext(bool load_buffer) {
  auto archive_stream = stream_.lock();
  if (!arch_stream_ && !archive_stream) return nullptr;

  auto entry = std::unique_ptr<wxArchiveEntry>(arch_stream_->GetNextEntry());

  auto &stream = vec_.emplace_back(archive_stream, entry.get(), path_);

  if (load_buffer) stream.Load(*arch_stream_, entry.get());

  return &stream;
}

size_t Input::Size() const { return vec_.size(); }

void Input::Clear() { vec_.clear(); }

void Input::GetChild(std::vector<const ReadStreamBase *> &vec) const {
  InputGetChildHelper<const Input, const ReadStream, const ReadStreamBase>(this,
                                                                           vec);
}

void Input::GetChild(std::vector<ReadStreamBase *> &vec) {
  InputGetChildHelper<Input, ReadStream, ReadStreamBase>(this, vec);
}

}  // namespace wx_archive

}  // namespace file_handler

}  // namespace fmr
