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

#ifndef FMR_BITMAP_POSITION_CTRL
#define FMR_BITMAP_POSITION_CTRL

#include <fmr/common/bitmask.h>
#include <fmr/common/vector.h>
#include <wx/gdicmn.h>

namespace fmr {

namespace bitmap {

enum PositionFlags {
  kPositionAlignCenter = 0x01,
  kPositionVertical = 0x02,
  kPositionHorizontal = 0x04
};

DEFINE_BITMASK_TYPE(PositionFlags);
class PositionItemConst {
 public:
  virtual int GetWidth() const = 0;
  virtual int GetHeight() const = 0;
  virtual int GetX() const = 0;
  virtual int GetY() const = 0;
};

class PositionItem : public PositionItemConst {
 public:
  virtual void SetX(int pos) = 0;
  virtual void SetY(int pos) = 0;
};

template <typename Parent, typename Object>
class PositionItemRefConstBase : public Parent {
 protected:
  Object *obj_;

 public:
  PositionItemRefConstBase(Object &obj) : obj_(&obj){};
  int GetWidth() const override { return obj_->GetWidth(); }
  int GetHeight() const override { return obj_->GetHeight(); }
  int GetX() const override { return obj_->GetX(); }
  int GetY() const override { return obj_->GetY(); }

  Object *GetObject() { return obj_; }
};

template <typename Parent, typename Object>
class PositionItemRefBase : public PositionItemRefConstBase<Parent, Object> {
 protected:
  using PositionItemRefConstBase<Parent, Object>::obj_;

 public:
  PositionItemRefBase(Object &obj)
      : PositionItemRefConstBase<Parent, Object>(obj) {}

  void SetX(int pos) override { return obj_->SetX(pos); }
  void SetY(int pos) override { return obj_->SetY(pos); }

  using PositionItemRefConstBase<Parent, Object>::GetObject;
};

template <typename T>
class PositionItemRefConst
    : public PositionItemRefConstBase<PositionItemConst, const T> {
 public:
  PositionItemRefConst(const T &obj)
      : PositionItemRefConstBase<PositionItemConst, const T>(obj) {}
};

template <typename T>
class PositionItemRef : public PositionItemRefBase<PositionItem, T> {
 public:
  PositionItemRef(T &obj) : PositionItemRefBase<PositionItem, T>(obj) {}
};

typedef std::vector<PositionItem *> PositionVector;
typedef std::vector<PositionItemConst *> PositionVectorConst;

class PositionCtrl {
 private:
  wxSize window_size_;
  wxSize minimum_size_;
  PositionFlags flags_;

 public:
  PositionCtrl(PositionFlags flags);

  void SetFlags(PositionFlags flags) { flags_ = flags; }
  PositionFlags GetFlags() const { return flags_; }

  void SetMinimumSize(const wxSize &size) { minimum_size_ = size; }
  const wxSize &GetMinimumSize() const { return minimum_size_; }

  /**
   * @brief: Recalculating the position of the vec_item
   * @param: vec_item vector of object to calculate
   */
  void RecalcPosition(const PositionVector &vec_item) const;
  /**
   * @brief: overload template method to automatically convert to PositionVector
   * @param: vec PositionItemRef must be constructable from T
   */
  template <typename T>
  void RecalcPosition(std::vector<T> &vec) const {
    std::vector<PositionItemRef<T>> vec_ref(vec.begin(), vec.end());
    std::vector<PositionItemRef<T> *> vec_ptr = Vector::ConvertToPtr(vec_ref);
    const PositionVector vec_item(vec_ptr.begin(), vec_ptr.end());
    return RecalcPosition(vec_item);
  }

  /**
   * @brief: G
   *
   * @param: vec_item Vector to calculate
   * @return:the size of the vector, wxDefaultSize if empty
   */
  wxSize GetSize(const PositionVectorConst &vec_item) const;

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
  wxSize GetMinimumItemSize(const PositionVectorConst &vec_item) const;

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

  void SetWindowSize(const wxSize &size) { window_size_ = size; }
  const wxSize &GetWindowSize() const { return window_size_; }
};

}  // namespace bitmap

}  // namespace fmr

#endif /* end of include guard: FMR_BITMAP_POSITION_CTRL */
