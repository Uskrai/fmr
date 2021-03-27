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

#ifndef FMR_FILE_HANDLER_INPUT_ITERATOR
#define FMR_FILE_HANDLER_INPUT_ITERATOR

#include <iterator>
#include <memory>

namespace fmr {

namespace file_handler {

// template <typename StreamType>
// class InputIterator {
// public:
// using value_type = StreamType;
// using reference = value_type &;
// using pointer = value_type *;
// using difference_type = std::ptrdiff_t;
// using iterator_category = std::random_access_iterator_tag;
//
// using It = InputIterator;
//
// protected:
// StreamType *ptr_;
//
// public:
// InputIterator(pointer ptr) { ptr_ = ptr; }
//
// reference operator*() { return *ptr_; }
// pointer operator->() { return ptr_; }
//
// // operator bool() { return ptr_; }
//
// bool operator==(const It &oth) { return ptr_ == oth.ptr_; }
// bool operator!=(const It &oth) { return !(*this == oth); }
//
// It &operator++() {
// ++ptr_;
// return *this;
// }
//
// It &operator--() {
// --ptr_;
// return *this;
// }
//
// It operator++(int) {
// It temp(*this);
// ++(*this);
// return temp;
// }
//
// It operator--(int) {
// It temp(*this);
// --(*this);
// return temp;
// }
//
// It &operator+=(const difference_type &mv) {
// ptr_ += mv;
// return *this;
// }
//
// It operator+(const difference_type &mv) {
// It temp(ptr_);
// return temp += mv;
// }
//
// It &operator-=(const difference_type &mv) {
// ptr_ -= mv;
// return *this;
// }
//
// It operator-(const difference_type &mv) {
// It temp(ptr_);
// return temp -= mv;
// }
//
// difference_type operator-(const It &oth) { return ptr_ - oth.ptr_; }
//
// pointer operator[](const difference_type &mv) { return It(ptr_ + mv); }
//
// bool operator<(const It &oth) { return oth - *this > 0; }
// bool operator>(const It &oth) { return oth < *this; }
//
// bool operator>=(const It &oth) { return !(*this < oth); }
// bool operator<=(const It &oth) { return !(*this > oth); }
//
// pointer GetPtr() { return ptr_; }
// };

namespace internal {
struct EmptyInputIterator {};
//
}  // namespace internal

template <typename StreamType,
          typename IteratorBase = internal::EmptyInputIterator>
class InputIteratorBaseHelper : public IteratorBase {
 public:
  using value_type = StreamType;
  using reference = value_type &;
  using pointer = value_type *;
  using difference_type = std::ptrdiff_t;
  using iterator_category = std::random_access_iterator_tag;

  using It = InputIteratorBaseHelper;
  using ItBase = IteratorBase;

  using Base = IteratorBase;

 public:
  virtual ~InputIteratorBaseHelper() {}
  virtual reference operator*() = 0;
  virtual pointer operator->() = 0;

  virtual pointer GetPtr() = 0;
  virtual pointer GetPtr() const = 0;

  // operator bool() { return ptr_; }

  virtual bool operator==(const It &oth) { return GetPtr() == oth.GetPtr(); }
  bool operator!=(const It &oth) { return !(*this == oth); }

  virtual It &operator++() = 0;

  virtual It &operator--() = 0;

  virtual It *operator++(int) = 0;
  virtual It *operator--(int) = 0;

  virtual It &operator+=(const difference_type &mv) = 0;

  virtual It *operator+(const difference_type &mv) = 0;

  virtual It &operator-=(const difference_type &mv) = 0;

  virtual It *operator-(const difference_type &mv) = 0;

  // virtual difference_type operator-(const ItBase &oth) {
  // return GetPtr() - oth.GetPtr();
  // };
  // virtual difference_type operator-(const It &oth) = 0;

  virtual It *operator[](const difference_type &mv) = 0;

  bool operator<(const It &oth) { return oth - *this > 0; }
  bool operator>(const It &oth) { return oth < *this; }

  bool operator>=(const It &oth) { return !(*this < oth); }
  bool operator<=(const It &oth) { return !(*this > oth); }
};

template <typename StreamType, typename IteratorBase>
class InputIteratorHelper : public IteratorBase {
 public:
  using value_type = StreamType;
  using reference = value_type &;
  using pointer = value_type *;
  using difference_type = std::ptrdiff_t;
  using iterator_category = std::random_access_iterator_tag;

  using It = InputIteratorHelper;
  using ItBase = IteratorBase;

  using Base = IteratorBase;

 private:
  pointer ptr_;

 public:
  InputIteratorHelper(const pointer ptr) { ptr_ = ptr; }

  virtual reference operator*() { return *ptr_; }
  virtual pointer operator->() { return ptr_; }

  virtual pointer GetPtr() { return ptr_; }
  virtual pointer GetPtr() const { return ptr_; }

  virtual It &operator++() {
    ++ptr_;
    return *this;
  }

  virtual It &operator--() {
    ++ptr_;
    return *this;
  }

  virtual It *operator++(int) {
    It *temp = new It(*this);
    ++(*this);
    return temp;
  }

  virtual It *operator--(int) {
    It *temp = new It(*this);
    --(*this);
    return temp;
  }

  virtual It &operator+=(const difference_type &mv) {
    ptr_ += mv;
    return *this;
  }

  virtual It *operator+(const difference_type &mv) { return new It(ptr_ + mv); }

  virtual It &operator-=(const difference_type &mv) {
    ptr_ += mv;
    return *this;
  }

  virtual It *operator-(const difference_type &mv) { return new It(ptr_ - mv); }

  // virtual difference_type operator-(const It &oth) = 0;
  // virtual difference_type operator-(const ItBase &oth) {
  // return ptr_ -= oth.ptr_;
  // }

  virtual It *operator[](const difference_type &mv) {
    return new It(ptr_ + mv);
  }

  // bool operator<(const It &oth) { return oth - *this > 0; }
  // bool operator>(const It &oth) { return oth < *this; }
  //
  // bool operator>=(const It &oth) { return !(*this < oth); }
  // bool operator<=(const It &oth) { return !(*this > oth); }
};

template <typename IteratorPointer>
class InputIterator {
 public:
  using value_type = typename IteratorPointer::value_type;
  using reference = value_type &;
  using pointer = value_type *;
  using difference_type = std::ptrdiff_t;
  using iterator_category = std::random_access_iterator_tag;

  using It = InputIterator;
  using ItBase = IteratorPointer;

 protected:
  std::unique_ptr<ItBase> it_;

 public:
  InputIterator(ItBase *it) { it_ = std::unique_ptr<ItBase>(it); };

  reference operator*() { return **it_; }
  pointer operator->() { return it_->GetPtr(); }

  // operator bool() { return ptr_; }

  bool operator==(const It &oth) { return *it_ == *oth.it_; }
  bool operator!=(const It &oth) { return !(*this == oth); }

  It &operator++() {
    ++(*it_);
    return *this;
  }

  It &operator--() {
    ++(*it_);
    return *this;
  }

  It operator++(int) {
    It temp((*it_)++);
    return temp;
  }

  It operator--(int) {
    It temp((*it_)--);
    return temp;
  }

  It &operator+=(const difference_type &mv) {
    (*it_) += mv;
    return *this;
  }

  It operator+(const difference_type &mv) {
    It temp((*it_) + mv);
    return temp;
  }

  It &operator-=(const difference_type &mv) {
    (*it_) -= mv;
    return *this;
  }

  It operator-(const difference_type &mv) {
    It temp((*it_) + mv);
    return temp -= mv;
  }

  difference_type operator-(const It &oth) {
    return it_->GetPtr() - oth.it_->GetPtr();
  }

  It operator[](const difference_type &mv) { return It((*it_)[mv]); }

  bool operator<(const It &oth) { return oth - *this > 0; }
  bool operator>(const It &oth) { return oth < *this; }

  bool operator>=(const It &oth) { return !(*this < oth); }
  bool operator<=(const It &oth) { return !(*this > oth); }

  pointer GetPtr() { return it_->GetPtr(); }
};

template <typename SteamType>
using ConstInputIterator = InputIterator<const SteamType>;

}  // namespace file_handler

}  // namespace fmr

#endif /* end of include guard: FMR_FILE_HANDLER_INPUT_ITERATOR */
