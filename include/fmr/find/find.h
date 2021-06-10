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

#ifndef INCLUDE_FMR_FIND_FIND_H_
#define INCLUDE_FMR_FIND_FIND_H_

#include <fmr/compare/comparer.h>
#include <fmr/find/checker.h>
#include <fmr/task/task.h>

namespace fmr {

namespace find {

template <typename = void>
class Find;

template <>
class Find<void> : public task::Task {
  bool recursive_ = false;

 public:
  /**
   * Set if Find should be Recursive or not
   */
  void SetRecursive(bool recursive) { recursive_ = recursive; }

  /**
   * Check if Find recursive
   */
  bool IsRecursive() const { return recursive_; }
};

template <typename T>
class Find : public Find<void> {
  Checker<T> *checker_ = nullptr;
  compare::Comparer<T> *comparer_ = nullptr;

 public:
  Find(Checker<T> *checker) : checker_(checker) {}

  // void SetChecker(Checker<T> *checker) { checker_ = checker; }
  /**
   * Set Comparer used for sorting child
   */
  void SetComparer(compare::Comparer<T> *comparer) { comparer_ = comparer; }

  /**
   * Get Checker
   */
  Checker<T> *GetChecker() const { return checker_; }

  /**
   * Get Compare
   */
  auto GetComparer() { return comparer_; }

  bool Check(const T &check) { return checker_->Check(check); }
};

}  // namespace find

}  // namespace fmr

#endif  // INCLUDE_FMR_FIND_FIND_H_
