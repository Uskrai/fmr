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

#include <fmr/file_handler/input_iterator.h>

namespace fmr {

namespace file_handler {

namespace internal {
struct EmptyInputBase {
  using iterator_pointer = EmptyInputIterator;
  using const_iterator_pointer = const EmptyInputIterator;
};
}  // namespace internal

template <typename StreamType, typename HandlerBase = internal::EmptyInputBase>
class InputBaseHelper : public HandlerBase {
 public:
  using value_type = StreamType;
  using pointer = value_type *;
  using const_pointer = const value_type *;
  using reference = value_type &;
  using const_reference = const value_type &;
  using size_type = std::size_t;

  using iterator_pointer =
      InputIteratorBaseHelper<value_type,
                              typename HandlerBase::iterator_pointer>;
  using const_iterator_pointer =
      InputIteratorBaseHelper<const value_type,
                              typename HandlerBase::const_iterator_pointer>;

  using iterator = InputIterator<iterator_pointer>;
  using const_iterator = ConstInputIterator<const_iterator_pointer>;
  using difference_type = typename iterator::difference_type;

  iterator begin() { return iterator(DoBegin()); }
  iterator end() { return iterator(DoEnd()); }
  const_iterator begin() const { return const_iterator(DoBegin()); }
  const_iterator end() const { return const_iterator(DoEnd()); }

 private:
  virtual const_iterator_pointer *DoBegin() const = 0;
  virtual const_iterator_pointer *DoEnd() const = 0;

  virtual iterator_pointer *DoBegin() = 0;
  virtual iterator_pointer *DoEnd() = 0;
};

}  // namespace file_handler

}  // namespace fmr
