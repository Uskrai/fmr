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

#include "fmr/compare/natural.h"

namespace fmr {

namespace compare {

String GetNonZero(const String& str, size_t idx, char chr) {
  size_t sz = 0;

  // skip zero
  while (str[idx] == '0') idx++;

  // count digit after zero
  while (isdigit(str[idx + sz])) sz++;

  const String& tmp = str.Substr(idx, sz);
  // skip the digit
  idx += sz;
  chr = str[idx];

  return (tmp.Get() == "") ? String("0") : String(tmp);
}

bool Natural::Compare(const String& s1, const String& s2) {
  // idx for first, pos for second;
  size_t idx = 0, pos = 0;
  while (s1.Exist(idx) && s2.Exist(pos)) {
    // this probably should be fixed
    char fst = s1[idx];
    char scnd = s2[pos];

    if (isdigit(fst) && isdigit(scnd)) {
      auto str1 = GetNonZero(s1, idx, fst);
      auto str2 = GetNonZero(s2, pos, scnd);

      // if str1's len not equal str2's len
      // will return whether str1 more or less than str2
      if (str1.Size() != str2.Size()) return str1.Size() < str2.Size();

      size_t i = 0;
      // only check str1 cuz the len is the same
      while (str1.Exist(i)) {
        // will return whether str1 more or less
        // than str2 if the value not equal
        if (str1[i] != str2[i]) return str1[i] < str2[i];
        ++i;
      }
    }

    if (fst != scnd || s1[idx] != s2[pos]) {
      if (idx != pos) return idx < pos;
      return fst < scnd;
    }

    ++idx;
    ++pos;
  }

  return s1.Size() < s2.Size();
}

}  // namespace compare

}  // namespace fmr
