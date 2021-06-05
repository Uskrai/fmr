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

ReadStream *Input::GetFirst(bool get_buffer) {
  ClearVector();
  iterator_item_ =
      std::make_unique<nwd::fs::directory_iterator>(Path::MakePath(GetPath()));

  return GetNext(get_buffer);
}

ReadStream *Input::GetNext(bool get_buffer) {
  if (!iterator_item_) return nullptr;

  if (*iterator_item_ == nwd::fs::end(*iterator_item_)) return nullptr;

  const nwd::fs::directory_entry &entry = **iterator_item_;
  ReadStream &stream = VectorEmplaceBack(entry.path(), get_buffer);
  ++(*iterator_item_);
  return &stream;
}

size_t Input::Index(const std::string &path) const {
  size_t idx = 0;
  for (const auto &it : GetVector()) {
    if (it.GetFullPath() == path) return idx;
    if (it.GetName() == path) return idx;

    ++idx;
  }
  return -1;
}

void Input::Clear() {
  iterator_item_ = nullptr;
  ClearVector();
}

}  // namespace filesystem

}  // namespace file_handler

}  // namespace fmr
