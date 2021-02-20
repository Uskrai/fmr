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

#ifndef FMR_WINDOW_GRID_CELL
#define FMR_WINDOW_GRID_CELL

#include <wx/gdicmn.h>
class wxDC;

namespace fmr {

namespace window {

class GridCell {
  wxRect rect_;
  bool is_selected_ = false;

 public:
  int GetX() const { return rect_.GetX(); }
  int GetY() const { return rect_.GetY(); }
  int GetWidth() const { return rect_.GetWidth(); }
  int GetHeight() const { return rect_.GetHeight(); }

  void SetY(int pos) { rect_.SetY(pos); }
  void SetX(int pos) { rect_.SetX(pos); }
  void SetHeight(int size) { rect_.SetHeight(size); }
  void SetWidth(int size) { rect_.SetWidth(size); }
  void SetSize(const wxSize &size) { rect_.SetSize(size); }
  void SetPosition(const wxPoint &pos) { rect_.SetPosition(pos); }

  void SetSelected(bool cond = true) { is_selected_ = cond; }
  bool IsSelected() const { return is_selected_; }

  virtual void Draw(wxDC &dc){};

  wxRect GetRect() const { return rect_; }
  wxSize GetSize() const { return rect_.GetSize(); }
  wxPoint GetPosition() const { return rect_.GetPosition(); }
};

}  // namespace window

}  // namespace fmr

#endif /* end of include guard: FMR_WINDOW_GRID_CELL */
