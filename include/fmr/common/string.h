/*
 *  Copyright (c) 2020-2021 Uskrai
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

#ifndef FMR_COMMON_STRING
#define FMR_COMMON_STRING

#include <wx/string.h>

#include <string>

namespace fmr {

namespace String {
template <typename T>
std::string ToString(const T &string) {
  return std::string(string.ToUTF8());
}

template <typename T>
T FromUTF8(const std::string &source, T &dest) {
  dest = T::FromUTF8(source.c_str());
  return dest;
}

template <class T>
T FromString(const std::string &source) {
  return T::FromUTF8(source.c_str());
}

};  // namespace String
};  // namespace fmr

#endif
