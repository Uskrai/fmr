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

#include <wx/gdicmn.h>

namespace fmr {

namespace explorer {

class Rectangle {
  wxRect rect_;

 public:
  Rectangle() {}
  Rectangle(int x, int y, int width, int height) : rect_(x, y, width, height) {}

  virtual int GetX() const { return rect_.GetX(); }
  virtual int GetY() const { return rect_.GetY(); }
  virtual int GetHeight() const { return rect_.GetHeight(); }
  virtual int GetWidth() const { return rect_.GetWidth(); }

  virtual void SetX(int pos) { rect_.SetX(pos); }
  virtual void SetY(int pos) { rect_.SetY(pos); }
  virtual void SetHeight(int size) { rect_.SetHeight(size); }
  virtual void SetWidth(int size) { rect_.SetWidth(size); }

  virtual wxSize GetSize() const { return wxSize(GetWidth(), GetHeight()); }
  virtual void SetSize(const wxSize &size) { rect_.SetSize(size); }

  virtual wxPoint GetPosition() const { return wxPoint(GetX(), GetY()); }
  virtual void SetPosition(const wxPoint &pos) { rect_.SetPosition(pos); }

  virtual wxRect GetRect() const { return rect_; }
  virtual void SetRect(const wxRect &rect) { rect_ = rect; }
};

}  // namespace explorer

}  // namespace fmr
