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

#ifndef FMR_POSITION_CTRL_BASE
#define FMR_POSITION_CTRL_BASE

#include <fmr/common/vector.h>
#include <fmr/position/item.h>
#include <wx/gdicmn.h>

#include "fmr/common/dimension.h"

namespace fmr {

namespace position {

typedef std::vector<PositionItem *> PositionVector;
typedef std::vector<PositionItemConst *> PositionVectorConst;

class CtrlBase {
  wxSize minimum_size_;
  wxSize window_size_;

 public:
  CtrlBase() {}

  /**
   * @brief: Recalculating the position of the vec_item
   * @param: vec_item vector of object to calculate
   */
  virtual void CalculatePosition(const PositionVector &vec) const = 0;
  /**
   * @brief: overload template method to automatically convert to PositionVector
   * @param: vec PositionItemRef must be constructable from T
   */
  template <typename T>
  void CalculatePosition(std::vector<T> &vec) const {
    std::vector<PositionItemRef<T>> vec_ref(vec.begin(), vec.end());
    std::vector<PositionItemRef<T> *> vec_ptr = Vector::ConvertToPtr(vec_ref);
    const PositionVector vec_item(vec_ptr.begin(), vec_ptr.end());
    return CalculatePosition(vec_item);
  }

  /**
   * @brief: Get size of the vector
   *
   * @param: vec_item Vector to calculate
   * @return:the size of the vector, wxDefaultSize if empty
   */
  virtual wxSize GetSize(const PositionVectorConst &vec) const {
    auto size = GetMinimumItemSize(vec);
    auto min_size = GetMinimumSize();

    dimension::Orientation orient = dimension::kVertical;
    int max;
    max = std::max(dimension::GetSize(min_size, orient),
                   dimension::GetSize(size, orient));
    dimension::SetSize(size, orient, max);

    orient = dimension::kHorizontal;
    max = std::max(dimension::GetSize(min_size, orient),
                   dimension::GetSize(size, orient));
    dimension::SetSize(size, orient, max);
    return size;
  }
  /**
   * @brief: overload template method to automatically convert to
   * PositionVectorConst
   * @param: vec PositionItemRefConst must be constructable from T
   * @return:
   */
  template <typename T>
  wxSize GetSize(const std::vector<T> &vec) const {
    std::vector<PositionItemRefConst<T>> vec_ref(vec.begin(), vec.end());
    std::vector<PositionItemRefConst<T> *> vec_ptr =
        Vector::ConvertToPtr(vec_ref);
    PositionVectorConst vec_item(vec_ptr.begin(), vec_ptr.end());
    return GetSize(vec_item);
  }

  /**
   * @brief:
   * @param: vec_item Vector to calculate
   * @return: the Size of the vector ignoring MinimumSize, wxDefaultSize
   * if empty
   */
  virtual wxSize GetMinimumItemSize(const PositionVectorConst &vec) const = 0;

  /**
   * @brief: overload template Method to automatically convert to
   * PositionVectorConst
   * @param: vec vvec PositionItemRefConst must be constructable from T
   * PositionItemRefConstBafe
   */
  template <typename T>
  wxSize GetMinimumItemSize(const std::vector<T> &vec) const {
    std::vector<PositionItemRefConst<T>> vec_ref(vec.begin(), vec.end());
    std::vector<PositionItemRefConst<T>> vec_ptr =
        Vector::ConvertToPtr(vec_ref);
    PositionVectorConst vec_item(vec_ptr.begin(), vec_ptr.end());
    return GetMinimumItemSize(vec_item);
  }

  void SetMinimumSize(const wxSize &size) { minimum_size_ = size; }
  const wxSize &GetMinimumSize() const { return minimum_size_; }

  void SetWindowSize(const wxSize &size) { window_size_ = size; }
  const wxSize &GetWindowSize() const { return window_size_; }
};

}  // namespace position

}  // namespace fmr

#endif /* end of include guard: FMR_POSITION_CTRL_BASE */
