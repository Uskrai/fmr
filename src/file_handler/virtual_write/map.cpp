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

#include "fmr/file_handler/virtual_write/map.h"

#include "fmr/file_handler/write_stream.h"

namespace fmr {

namespace file_handler {

namespace virtual_write {

bool Map::Add(const Stream &stream) {
  auto vec = Separate(stream.GetName());
  return Add(vec, stream);
}

bool Map::Exist(const std::string &name) const {
  return Search(name) != nullptr;
}

Map::StringVector Map::Separate(const std::string &name) const {
  StringVector ret;
  if (separator_ == '\0') {
    ret.push(name);
  } else {
    std::string current;
    for (auto it : name) {
      if (it == separator_) {
        ret.push(current);
        current.clear();
      } else {
        current += it;
      }
    }
    if (current != "") ret.push(current);
  }
  return ret;
}

bool Map::SetVirtualStream(const Stream &stream) {
  if (IsDirectory() != stream.IsDirectory()) return false;
  stream_ = stream;
  return true;
}

bool Map::Add(StringVector &vec, Stream stream) {
  if (!vec.empty()) {
    auto str = vec.front();
    vec.pop();

    auto child = Find(str);

    if (vec.empty()) {
      if (child && stream.GetWriteType() & kWriteOverwrite) {
        return child->SetVirtualStream(stream);
      } else if (!child) {
        path_.insert(std::make_pair(str, Map(separator_, stream)));
        return true;
      }
    } else if (child && child->IsDirectory()) {
      return child->Add(vec, stream);
    }
  }
  return false;
}

const Map *Map::Search(const Stream &stream) const {
  return Search(stream.GetName());
}

const Map *Map::Search(const std::string &name) const {
  auto vec = Separate(name);
  return Search(vec);
}

const Map *Map::Search(StringVector &vec) const {
  if (!vec.empty()) {
    auto str = vec.front();
    vec.pop();

    if (auto child = Find(str)) {
      if (vec.empty()) return child;
      return child->Search(vec);
    }
  }

  return nullptr;
}

const Map *Map::Find(const std::string &name) const {
  auto it = path_.find(name);
  if (it != path_.end()) {
    return &it->second;
  }
  return nullptr;
}

Map *Map::Find(const std::string &name) {
  auto it = path_.find(name);
  if (it != path_.end()) {
    return &it->second;
  }
  return nullptr;
}

std::vector<Stream> Map::ToVector(ExtractType type) const {
  std::vector<Stream> ret;
  ToVector(ret, type);
  return ret;
}

void Map::ToVector(std::vector<Stream> &vec, ExtractType type) const {
  auto should_push = [](const Stream &stream, ExtractType type) {
    bool ret = false;
    auto check = [&](bool val) { ret = ret || val; };

    check(stream.IsReadStream() && type & kExtractReadStream);
    check(stream.IsWriteStream() && type & kExtractWriteStream);
    check(!stream.IsReadStream() && !stream.IsWriteStream());

    return ret;
  };

  {
    auto &vir_stream = GetVirtualStream();
    if (should_push(vir_stream, type)) vec.push_back(vir_stream);
  }

  for (const auto &it : path_) {
    auto &map = it.second;
    map.ToVector(vec, type);
  }
}

}  // namespace virtual_write

}  // namespace file_handler

}  // namespace fmr
