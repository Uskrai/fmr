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

#ifndef FMR_COMMON_COMPARE
#define FMR_COMMON_COMPARE

#include <wx/arrstr.h>

namespace fmr {

#define DeclareStringCompareFunction(name)                     \
  bool name##Sortable(const Sortable &s1, const Sortable &s2); \
  bool name##String(const Compare::String &s1, const Compare::String &s2)

#define DefineStringCompareFunction(name)                       \
  bool name##Sortable(const Sortable &s1, const Sortable &s2) { \
    return name##String(s1.GetString(), s2.GetString());        \
  };                                                            \
  bool name##String(const String &s1, const String &s2)

namespace Compare {
typedef char Char;
typedef std::string String;

class Sortable {
 public:
  virtual String GetString() const = 0;
};

char ToLower(char ch);

bool NotEnd(const wxString &str, size_t idx);
String GetNonZero(const String &str, size_t &idx, Char &chr);
DeclareStringCompareFunction(Natural);
}  // namespace Compare

};  // namespace fmr
#endif
