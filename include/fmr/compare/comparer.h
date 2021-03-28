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

#ifndef FMR_COMPARE_SORT_COMPONENT
#define FMR_COMPARE_SORT_COMPONENT

#include <fmr/compare/sortable.h>
#include <fmr/compare/string.h>

#include <algorithm>
#include <memory>

namespace fmr {

namespace compare {

class Comparer {
  std::vector<std::shared_ptr<CharModifier>> modifier_;

 public:
  virtual bool Compare(const String &s1, const String &s2) = 0;

  template <typename Type, typename... U>
  void MakeModifier(U &&...u) {
    modifier_.push_back(std::make_shared<Type>(std::forward<U>(u)...));
  }

  bool operator()(const std::string &s1, const std::string &s2) {
    return Compare(s1, s2);
  };
  bool operator()(const Sortable &s1, const Sortable &s2) {
    return this->operator()(s1.GetString(), s2.GetString());
  };

  std::unique_ptr<Comparer> Clone() const {
    return std::unique_ptr<Comparer>(DoClone());
  }

 protected:
  virtual Comparer *DoClone() const = 0;
};

class ComparerSort {
  Comparer &cmp_;

 public:
  ComparerSort(Comparer &cmp) : cmp_(cmp) {}

  bool operator()(const Sortable &s1, const Sortable &s2) {
    return cmp_(s1, s2);
  }
};

template <typename RandomIt>
void Sort(RandomIt first, RandomIt last, const Comparer &compare) {
  auto ptr = compare.Clone();
  ComparerSort cmp(*ptr);
  std::sort(first, last, cmp);
}

}  // namespace compare

}  // namespace fmr

#endif /* end of include guard: FMR_COMPARE_SORT_COMPONENT */
