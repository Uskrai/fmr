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

#include <fmr/common/compare.h>

#include <algorithm>

namespace fmr {
namespace Compare {
bool NotEnd(const std::string& str, size_t idx) { return idx < str.size(); }

char ToLower(char ch) {
  return static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
}

String GetNonZero(const String& str, size_t& idx, Char& chr) {
  size_t sz = 0;

  // skip zero
  while (str[idx] == '0') idx++;

  // count digit after zero
  while (isdigit(str[idx + sz])) sz++;

  const String& tmp = str.substr(idx, sz);
  // skip the digit
  idx += sz;
  chr = str[idx];

  return (tmp == "") ? "0" : tmp;
}

DefineStringCompareFunction(Natural) {
  const String& first = s1;
  const String& second = s2;

  // const String& first = s1.Lower().wx_str();

  // const String& second = s2.Lower().wx_str();
  // idx for first, pos for second;
  size_t idx = 0, pos = 0;
  while (NotEnd(first, idx) && NotEnd(second, pos)) {
    // this probably should be fixed
    Char fst = ToLower(first[idx]);
    Char scnd = ToLower(second[pos]);

    if (isdigit(fst) && isdigit(scnd)) {
      const String& str1 = GetNonZero(first, idx, fst);
      const String& str2 = GetNonZero(second, pos, scnd);

      // if str1's len not equal str2's len
      // will return whether str1 more or less than str2
      if (str1.size() != str2.size()) return str1.size() < str2.size();

      size_t i = 0;
      // only check str1 cuz the len is the same
      while (NotEnd(str1, i)) {
        // will return whether str1 more or less
        // than str2 if the value not equal
        if (str1[i] != str2[i]) return str1[i] < str2[i];
        i++;
      }
    }
    if (fst != scnd || first[idx] != second[pos]) {
      if (idx != pos) return idx < pos;
      return fst < scnd;
    }

    idx++;
    pos++;
  }

  return first.size() < second.size();
}

}  // namespace Compare

}  // namespace fmr
