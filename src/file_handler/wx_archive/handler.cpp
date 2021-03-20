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

#include "fmr/file_handler/wx_archive/handler.h"

#include "fmr/common/path.h"
#include "fmr/file_handler/filesystem/handler.h"
#include "fmr/nowide/string.h"
#include "wx/archive.h"

namespace fmr {

namespace file_handler {

namespace wx_archive {

void Handler::Open(const std::string &path) {
  path_ = path;

  parent_ = std::make_unique<filesystem::Handler>(Path::GetParent(path));
}

void Handler::Open(const file_handler::ReadStream &stream) {
  stream_ = stream.Clone();
  input.Open(stream_);
}

bool Handler::IsHandleable(const std::string &str_path) const {
  return FindFactory(str_path);
}

bool Handler::IsHandleable(const Stream &stream) const { return false; }

bool Handler::IsHandleable(const file_handler::ReadStream &stream) const {
  return FindFactory(stream);
}

const wxArchiveClassFactory *Handler::FindFactory(
    const file_handler::ReadStream &stream) {
  return FindFactory(stream.GetName());
}

const wxArchiveClassFactory *Handler::FindFactory(const std::string &str_path) {
  const wxArchiveClassFactory *factory = nullptr;
  wxString path = String::Widen<wxString>(str_path);

  factory = wxArchiveClassFactory::Find(path, wxSTREAM_FILEEXT);
  if (!factory) {
    const wxFilterClassFactory *fcf;
    fcf = wxFilterClassFactory::Find(path, wxSTREAM_FILEEXT);

    if (fcf) {
      path = fcf->PopExtension(path);
      factory = wxArchiveClassFactory::Find(path, wxSTREAM_FILEEXT);
    }
  }

  return factory;
}

bool Handler::IsOk() const { return IsExist(path_) && IsHandleable(path_); }

bool Handler::IsExist(const std::string &path) const {
  return Path::Exist(path);
}

}  // namespace wx_archive

}  // namespace file_handler

}  // namespace fmr
