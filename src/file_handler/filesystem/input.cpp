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

#include "fmr/file_handler/input.h"

#include <fmr/common/vector.h>
#include <fmr/file_handler/filesystem/input.h>

namespace fmr {

namespace file_handler {

namespace filesystem {

Stream *Input::GetFirst(bool get_buffer) {
  vec_.clear();
  iterator_item_ =
      std::make_unique<nwd::fs::directory_iterator>(Path::MakePath(GetPath()));
  *iterator_item_ = nwd::fs::begin(*iterator_item_);

  return GetNext(get_buffer);
}

Stream *Input::GetNext(bool get_buffer) {
  if (!iterator_item_) return nullptr;

  if (*iterator_item_ == nwd::fs::end(*iterator_item_)) return nullptr;

  const nwd::fs::directory_entry &entry = **iterator_item_;
  Stream &stream = vec_.emplace_back(entry.path());
  return &stream;
}

void Input::GetChild(std::vector<StreamBase *> &vec) {
  InputGetChildHelper<Input, Stream, StreamBase>(this, vec);
}

void Input::GetChild(std::vector<const StreamBase *> &vec) const {
  InputGetChildHelper<const Input, const Stream, const StreamBase>(this, vec);
}

void Input::GetChild(std::vector<const Stream *> &vec) const {
  vec.reserve(vec_.size() + vec.size());
  for (auto &it : vec_) vec.push_back(&it);
}

void Input::GetChild(std::vector<Stream *> &vec) {
  vec.reserve(vec_.size() + vec.size());
  for (auto &it : vec_) vec.push_back(&it);
}

}  // namespace filesystem

}  // namespace file_handler

}  // namespace fmr
