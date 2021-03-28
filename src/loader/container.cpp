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

#include "fmr/loader/container.h"

namespace fmr {

namespace loader {

SStream *Container::AddFoundStream(const SStream *source_stream,
                                   const SStream &found_stream) {
  auto stream = std::make_unique<SStream>(found_stream);
  auto found_stream_ptr = stream.get();

  source_found_map_.Insert(source_stream, found_stream_ptr);
  loaded_stream_.push_back(std::move(stream));

  return found_stream_ptr;
}

std::vector<SStream *> Container::GetFoundStream(
    const SStream *source_stream) const {
  return source_found_map_.GetValue(source_stream);
}

bool Container::IsFound(const SStream *found_stream) const {
  return source_found_map_.IsValueExist(const_cast<SStream *>(found_stream));
}

bool Container::IsSourceFound(const SStream *source_stream) const {
  return source_found_map_.IsKeyExist(source_stream);
}

const SStream *Container::GetSourceStream(const SStream *found_stream) const {
  return source_found_map_.GetKey(const_cast<SStream *>(found_stream));
}

void Container::InsertFind(const SStream *source_stream) {
  find_queue_set_.Insert(source_stream);
}

void Container::RemoveFind(const SStream *source_stream) {
  find_queue_set_.Remove(source_stream);
}

bool Container::IsInFindQueue(const SStream *source_stream) const {
  return find_queue_set_.Exist(source_stream);
}

void Container::InsertLoad(const SStream *found_stream) {
  load_queue_set_.Insert(found_stream);
}

bool Container::IsInLoadQueue(const SStream *found_stream) const {
  return load_queue_set_.Exist(found_stream);
}

void Container::RemoveLoad(const SStream *found_stream) {
  return load_queue_set_.Remove(found_stream);
}

void Container::Clear() {
  source_found_map_.Clear();
  find_queue_set_.Clear();
  load_queue_set_.Clear();
  loaded_stream_.clear();
}

}  // namespace loader

}  // namespace fmr
