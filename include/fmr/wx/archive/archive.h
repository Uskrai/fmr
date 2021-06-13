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

#ifndef INCLUDE_FMR_WX_ARCHIVE_ARCHIVE_H_
#define INCLUDE_FMR_WX_ARCHIVE_ARCHIVE_H_

#include <memory>
#include <string>

class wxInputStream;
class wxArchiveInputStream;
class wxArchiveClassFactory;

namespace fmr {

namespace wx {

namespace archive {

class Find;

class Archive {
  std::unique_ptr<wxArchiveInputStream> stream_;

 public:
  Archive(const std::string &path, wxInputStream &stream);
  Archive(const std::string &path, wxInputStream *stream);

  static const wxArchiveClassFactory *FindFactory(const std::string &filename);
  static bool CanHandle(const std::string &path);

 private:
  wxArchiveInputStream *GetInputStream() { return stream_.get(); }
  const wxArchiveInputStream *GetInputStream() const { return stream_.get(); }

  friend Find;
};

}  // namespace archive

}  // namespace wx

}  // namespace fmr

#endif  // INCLUDE_FMR_WX_ARCHIVE_ARCHIVE_H_
