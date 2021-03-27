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

bool Handler::Open(const std::string &path) {
  path_ = path;
  Write()->Open(path_);
  Read()->Open(path_);

  std::string parent_path = Path::GetParent(path_);

  if (!Path::IsRoot(path)) {
    parent_ = std::make_unique<Handler>(parent_path);
  }
  return true;
}

bool Handler::Open(const file_handler::ReadStream &stream) { return false; }

bool Handler::Open(const local::ReadStream &stream) {
  return Open(stream.GetFullPath());
}

std::string Handler::GetInternalName(const std::string &path) const {
  return Path::MakeRelative(GetPath(), path);
}

std::string Handler::GetExternalName(
    const file_handler::ReadStream &stream) const {
  return Path::Append(stream.GetHandlerPath(), stream.GetName());
}

bool Handler::IsOk() const {
  return path_ != "" && Path::Exist(GetPath()) && Path::IsDirectory(GetPath());
}

bool Handler::IsExist() const { return Path::Exist(GetPath()); }

}  // namespace filesystem

}  // namespace file_handler

}  // namespace fmr
