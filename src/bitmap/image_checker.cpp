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

#include "fmr/bitmap/image_checker.h"

#include <wx/log.h>

#include <regex>

#include "fmr/bitmap/image_util.h"
#include "fmr/handler/stream_util.h"
#include "fmr/nowide/string.h"
#include "fmr/queue/find_handler.h"

namespace fmr {

namespace bitmap {

queue::FindStatus ImageChecker::Check(queue::FindHandler &parent,
                                      AbstractHandler &handler,
                                      SStream &stream) {
  std::string ext = String::Narrow(wxImage::GetImageExtWildcard());
  ext = ext.substr(ext.find_first_of('(') + 1, ext.find_last_of(')') - 1);
  while (ext.size()) {
    size_t separator = ext.find_first_of(';');
    // separate extension
    std::string temp = ext.substr(1, separator - 1) + "$";

    try {
      std::regex re(temp);
      std::smatch c;
      bool res = std::regex_search(stream.GetName(), c, re);
      if (res) return queue::kFindItemFound;
    } catch (std::regex_error &err) {
      handler.GetStream(stream);
      if (image_util::CanRead(stream)) return queue::kFindItemFound;
      return queue::kFindNotFound;
    }

    if (separator == std::string::npos) break;
    ext = ext.substr(separator + 1);
  }
  return queue::kFindNotFound;
}

queue::FindStatus ImageChecker::Check(queue::FindHandler &parent,
                                      AbstractOpenableHandler &handler,
                                      SStream &stream) {
  if (image_util::CanRead(stream_util::GetPath(stream))) {
    return queue::kFindItemFound;
  }

  return Check(parent, static_cast<AbstractHandler &>(handler), stream);
}

}  // namespace bitmap

}  // namespace fmr
