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

#ifndef FMR_POSITION_ITEM
#define FMR_POSITION_ITEM

namespace fmr {

namespace position {

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

}  // namespace position

}  // namespace fmr

#endif /* end of include guard: FMR_POSITION_ITEM */
