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

#include <fmr/wx/archive/archive.h>
#include <wx/archive.h>

namespace fmr {

namespace wx {

namespace archive {

template <typename InputStream>
std::unique_ptr<wxArchiveInputStream> NewStream(const std::string& filename,
                                                InputStream&& stream) {
  auto factory = Archive::FindFactory(filename);
  return std::unique_ptr<wxArchiveInputStream>(factory->NewStream(stream));
}

Archive::Archive(const std::string& filename, wxInputStream& stream) {
  stream_ = NewStream(filename, stream);
}

Archive::Archive(const std::string& filename, wxInputStream* stream) {
  stream_ = NewStream(filename, stream);
}

const wxArchiveClassFactory* Archive::FindFactory(const std::string& filename) {
  using Factory = wxArchiveClassFactory;
  using FilterFactory = wxFilterClassFactory;
  wxString string = wxString::FromUTF8(filename.c_str());

  auto fcf = FilterFactory::Find(string, wxSTREAM_FILEEXT);
  if (fcf) {
    string = fcf->PopExtension(string);
  }

  return Factory::Find(string, wxSTREAM_FILEEXT);
}

bool Archive::CanHandle(const std::string& filename) {
  return FindFactory(filename);
}

}  // namespace archive

}  // namespace wx

}  // namespace fmr
