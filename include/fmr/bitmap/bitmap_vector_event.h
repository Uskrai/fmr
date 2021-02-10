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

namespace fmr {

#include <wx/event.h>

#include "fmr/bitmap/bitmap_page.h"

namespace bitmap {

class BitmapVectorEvent : public wxNotifyEvent {
 private:
  BitmapVector *vec_ = nullptr;
  size_t pos_ = 0;
  wxDirection direction_ = wxDOWN;

 public:
  BitmapVectorEvent(wxEventType type, int id) : wxNotifyEvent(type, id) {}
  BitmapVectorEvent(const BitmapVectorEvent &other) : wxNotifyEvent(other) {
    vec_ = other.vec_;
    pos_ = other.pos_;
    direction_ = other.direction_;
  }

  BitmapVectorEvent *Clone() const override {
    return new BitmapVectorEvent(*this);
  }

  void SetBitmapVec(BitmapVector *vec) { vec_ = vec; }
  void SetPagePos(size_t pos) { pos_ = pos; }
  void SetDirection(wxDirection direction) { direction_ = direction; }

  BitmapVector *GetBitmapVec() { return vec_; }
  size_t GetPagePos() const { return pos_; }
  wxDirection GetDirection() { return direction_; }
};

}  // namespace bitmap

}  // namespace fmr
