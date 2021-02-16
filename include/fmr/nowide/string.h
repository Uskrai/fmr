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

#if 1

#include <nowide/convert.hpp>

#else
#include <wx/string.h>
#endif

#if defined(wxUSE_UNICODE) || defined(wxUSE_UNICODE_WCHAR) || \
    defined(wxUSE_UNICODE_UTF8)
#define FMR_USE_WXWIDGETS
#include <wx/string.h>

#include <iostream>
#endif

namespace fmr {

namespace String {

#if 1

template <typename T>
std::string Narrow(const T &string) {
  return nowide::narrow(string);
}

template <typename T>
T Widen(const std::string &source) {
  return nowide::widen(source);
}

#else
#include <string>
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
#endif

#ifdef FMR_USE_WXWIDGETS
template <>
inline std::string Narrow<wxString>(const wxString &string) {
  return nowide::narrow(string.ToStdWstring());
  return std::string(string.ToUTF8());
}

#endif  // end of ifde FMR_USE_WXWIDGETS

};  // namespace String
};  // namespace fmr

#endif
