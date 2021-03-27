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

#ifndef FMR_FILE_HANDLER_INPUT_IMPLEMENTER_HELPER
#define FMR_FILE_HANDLER_INPUT_IMPLEMENTER_HELPER

#include <fmr/compare/comparer.h>
#include <fmr/file_handler/input_iterator.h>

#include <vector>

namespace fmr {

namespace file_handler {

template <typename StreamType, typename HandlerBase>
class InputImplementHelper : public HandlerBase {
 public:
  using container_type = std::vector<StreamType>;
  using value_type = StreamType;
  using pointer = value_type *;
  using const_pointer = const value_type *;
  using reference = value_type &;
  using iterator_pointer =
      InputIteratorHelper<value_type, typename HandlerBase::iterator_pointer>;

  using const_iterator_pointer =
      InputIteratorHelper<const value_type,
                          typename HandlerBase::const_iterator_pointer>;

  using iterator = InputIterator<iterator_pointer>;
  using const_iterator = ConstInputIterator<const_iterator_pointer>;

  iterator begin() { return iterator(DoBegin()); }
  iterator end() { return iterator(DoEnd()); }

  const_iterator begin() const { return const_iterator(DoBegin()); }
  const_iterator end() const { return const_iterator(DoEnd()); }

  size_t Size() const override { return vec_.size(); }

  pointer At(size_t idx) override { return &vec_[idx]; }
  const_pointer At(size_t idx) const override { return &vec_[idx]; }

  bool IsEmpty() const override { return vec_.empty(); }

  void Sort(const compare::Comparer &compare) override {
    compare::Sort(vec_.begin(), vec_.end(), compare);
  }

 private:
  container_type vec_;

 protected:
  container_type &GetVector() { return vec_; }
  const container_type &GetVector() const { return vec_; }

  void ClearVector() { return vec_.clear(); }

  template <typename T>
  reference VectorPushBack(T &&item) {
    return vec_.push_back(std::forward<T>(item));
  }

  template <typename... U>
  reference VectorEmplaceBack(U &&...u) {
    return vec_.emplace_back(std::forward<U>(u)...);
  }

 private:
  const_iterator_pointer *DoBegin() const override {
    return new const_iterator_pointer(vec_.data());
    // return nullptr;
  }
  const_iterator_pointer *DoEnd() const override {
    return new const_iterator_pointer(vec_.data() + vec_.size());
    // return nullptr;
  }

  iterator_pointer *DoBegin() override {
    // return nullptr;
    return new iterator_pointer(vec_.data());
  }
  iterator_pointer *DoEnd() override {
    // return nullptr;
    return new iterator_pointer(vec_.data() + vec_.size());
  }
};

}  // namespace file_handler

}  // namespace fmr

#endif /* end of include guard: FMR_FILE_HANDLER_INPUT_IMPLEMENTER_HELPER */
