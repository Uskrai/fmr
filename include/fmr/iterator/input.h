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

#ifndef INCLUDE_FMR_FS_ITERATOR_H_
#define INCLUDE_FMR_FS_ITERATOR_H_

#include <memory>

namespace fmr {

namespace iterator {

class Entry;

template <typename T>
class BaseInput {
 public:
  virtual ~BaseInput() {}
  virtual BaseInput &operator++() = 0;
  virtual const T &operator*() const = 0;

  bool operator==(const BaseInput &o) {
    return typeid(*this) == typeid(o) && equal(o);
  }

 protected:
  virtual bool equal(const BaseInput &o) const { return true; }
};

template <typename T>
class Input {
  std::unique_ptr<BaseInput<T>> ptr_;

 public:
  using value_type = T;

  Input() {}
  Input(std::unique_ptr<BaseInput<T>> ptr) : ptr_{std::move(ptr)} {}
  Input(Input &&o) { *this = std::move(o); }

  Input &operator=(Input &&o) {
    ptr_ = std::move(o.ptr_);
    return *this;
  }

  const T &operator*() const { return **ptr_; }

  Input &operator++() {
    ++(*ptr_);
    return *this;
  }

  bool operator==(const Input &o) const {
    return (ptr_ == o.ptr_) || (*ptr_ == *o.ptr_);
  }

  BaseInput<T> *ptr() { return ptr_.get(); }

  bool operator!=(const Input &o) const { return !((*this) == o); }
};

template <typename T>
class InputContainer {
 public:
  virtual ~InputContainer() {}
  virtual Input<T> &iterator() = 0;
  virtual Input<T> &end() = 0;
};

}  // namespace iterator

}  // namespace fmr

#endif  // INCLUDE_FMR_FS_ITERATOR_H_
