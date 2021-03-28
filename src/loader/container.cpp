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

ReadStream *Container::AddFoundStream(
    const ReadStream *source_stream, std::unique_ptr<ReadStream> found_stream) {
  auto ret = found_stream.get();

  source_found_map_.Insert(source_stream, ret);
  loaded_stream_.push_back(std::move(found_stream));

  return ret;
}

std::vector<ReadStream *> Container::GetFoundStream(
    const ReadStream *source_stream) const {
  return source_found_map_.GetValue(source_stream);
}

bool Container::IsFound(const ReadStream *found_stream) const {
  return source_found_map_.IsValueExist(const_cast<ReadStream *>(found_stream));
}

bool Container::IsSourceFound(const ReadStream *source_stream) const {
  return source_found_map_.IsKeyExist(source_stream);
}

const ReadStream *Container::GetSourceStream(
    const ReadStream *found_stream) const {
  return source_found_map_.GetKey(const_cast<ReadStream *>(found_stream));
}

void Container::InsertFind(const ReadStream *source_stream) {
  find_queue_set_.Insert(source_stream);
}

void Container::RemoveFind(const ReadStream *source_stream) {
  find_queue_set_.Remove(source_stream);
}

bool Container::IsInFindQueue(const ReadStream *source_stream) const {
  return find_queue_set_.Exist(source_stream);
}

void Container::InsertLoad(const ReadStream *found_stream) {
  load_queue_set_.Insert(found_stream);
}

bool Container::IsInLoadQueue(const ReadStream *found_stream) const {
  return load_queue_set_.Exist(found_stream);
}

void Container::RemoveLoad(const ReadStream *found_stream) {
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
