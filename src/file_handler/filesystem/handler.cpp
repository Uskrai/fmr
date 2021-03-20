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

#include "fmr/file_handler/filesystem/handler.h"

namespace fmr {

namespace file_handler {

namespace filesystem {

Handler::Handler() = default;

void Handler::Open(const std::string &path) {
  path_ = path;
  Write()->Open(path_);
  Read()->Open(path_);

  std::string parent_path = Path::GetParent(path_);

  if (!Path::IsRoot(path)) {
    parent_ = std::make_unique<Handler>(parent_path);
  }
}

bool Handler::IsOk() const {
  return path_ != "" && Path::Exist(GetPath()) && !Path::IsDirectory(GetPath());
}

bool Handler::IsExist(const std::string &name) const {
  return Path::Exist(name);
}

}  // namespace filesystem

}  // namespace file_handler

}  // namespace fmr
