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

#ifndef INCLUDE_FMR_COMPARE_COMPARER_H_
#define INCLUDE_FMR_COMPARE_COMPARER_H_

#include <fmr/compare/comparator.h>

#include <algorithm>
#include <memory>
#include <vector>

namespace fmr {

namespace compare {

template <typename T>
class Comparer {
  std::vector<std::shared_ptr<Comparator<T>>> comparator_;

 public:
  Comparer();

  void AddComparator(std::shared_ptr<Comparator<T>> comparator) {
    comparator_.push_back(comparator);
  }

  bool operator()(const T& t1, const T& t2) {
    int v = false;
    for (auto comparator : comparator_) {
      if (!comparator) continue;
      v = comparator->Compare(t1, t2);
      if (v != 0) return 0 < v;
    }
    return true;
  }

  template <typename RandomIt>
  void Sort(RandomIt first, RandomIt last) {
    std::sort(first, last, *this);
  }

  template <typename ExecutionPolicy, typename RandomIt>
  void Sort(ExecutionPolicy&& policy, RandomIt first, RandomIt last) {
    std::sort(std::forward<ExecutionPolicy>(policy), first, last, *this);
  }
};

}  // namespace compare

}  // namespace fmr
#endif  // INCLUDE_FMR_COMPARE_COMPARER_H_
