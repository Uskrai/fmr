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

#ifndef FMR_COMPARE_STRING
#define FMR_COMPARE_STRING

#include <fmr/compare/char_modifier.h>

#include <memory>
#include <string>
#include <vector>

namespace fmr {

namespace compare {

class String {
  std::string str_;
  std::vector<std::shared_ptr<CharModifier>> vec_;

 public:
  String(const std::string &string) : str_(string){};

  void AddModifier(std::shared_ptr<CharModifier> modifier) {
    vec_.push_back(std::move(modifier));
  }

  const std::string &Get() const { return str_; }

  char operator[](size_t pos) const {
    char ch = str_[pos];
    for (auto &it : vec_) it->Modify(ch);
    return ch;
  };

  bool Exist(size_t size) const { return size < Size(); }
  size_t Size() const { return str_.size(); }

  String Substr(size_t pos = 0, size_t count = std::string::npos) const {
    return String(str_.substr(pos, count));
  }
};

}  // namespace compare

}  // namespace fmr

#endif /* end of include guard: FMR_COMPARE_STRING */
