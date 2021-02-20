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

#ifndef FMR_WINDOW_GRID_ITEM
#define FMR_WINDOW_GRID_ITEM

#include <fmr/window/grid_cell.h>
#include <wx/gdicmn.h>

#include <memory>

namespace fmr {

namespace window {

class GridCell;

class GridItem {
  std::unique_ptr<GridCell> cell_;
  wxSize border_size_{0, 0};
  wxPoint pos_;
  size_t highlight_width_ = 5;
  int highlight_offset_ = 10;

 public:
  GridItem(const wxSize &border_size) {
    cell_ = std::make_unique<GridCell>();
    SetBorderSize(border_size_);
  }
  GridItem(std::unique_ptr<GridCell> cell) { cell_ = std::move(cell); };

  void SetBorderSize(const wxSize &size) {
    border_size_ = size;
    SetX(GetX());
    SetY(GetY());
  }
  wxSize GetBorderSize() const { return border_size_; }

  int GetWidth() const {
    return cell_->GetWidth() + border_size_.GetWidth() * 2;
  }
  int GetHeight() const {
    return cell_->GetHeight() + border_size_.GetHeight() * 2;
  }
  int GetX() const { return pos_.x; }
  int GetY() const { return pos_.y; }

  wxSize GetSize() { return wxSize(GetWidth(), GetHeight()); }
  wxPoint GetPosition() { return pos_; }
  wxRect GetRect() { return wxRect(pos_, GetSize()); }

  void SetX(int pos) {
    pos_.x = pos;
    cell_->SetX(pos + border_size_.GetWidth());
  }

  void SetY(int pos) {
    pos_.y = pos;
    cell_->SetY(pos + border_size_.GetHeight());
  }

  GridCell *GetCell() { return cell_.get(); }
  const GridCell *GetCell() const { return cell_.get(); }

  void SetSelected(bool selected = true) { GetCell()->SetSelected(selected); }
  bool IsSelected() const { return GetCell()->IsSelected(); }

  void DrawHighlight(wxDC &dc);
  void Draw(wxDC &dc);
};

}  // namespace window

}  // namespace fmr

#endif /* end of include guard: FMR_WINDOW_GRID_ITEM */
