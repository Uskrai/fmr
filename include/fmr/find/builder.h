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

#ifndef INCLUDE_FMR_FIND_BUILDER_H_
#define INCLUDE_FMR_FIND_BUILDER_H_

#include <fmr/find/checker.h>

namespace fmr {

namespace find {

/**
 * This class is a builder class for Find class
 */
class Builder {
  bool recursive_;
  Checker *checker_;

 public:
  Builder(){};
  virtual ~Builder(){};

  void SetRecursive(bool recursive) { recursive_ = recursive; }
  void SetChecker(Checker *checker) { checker_ = checker; }

  bool IsRecursive() { return recursive_; }
  Checker *GetChecker() { return checker_; }

 private:
  /* data */
};

}  // namespace find

}  // namespace fmr

#endif  // INCLUDE_FMR_FIND_BUILDER_H_
