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

#include "fmr/window/grid_window.h"

#include "fmr/common/dimension.h"
#include "fmr/common/vector.h"
#include "fmr/position/grid_ctrl.h"
#include "fmr/position/item.h"
#include "fmr/window/grid_cell.h"
#include "fmr/window/grid_item.h"
namespace fmr {

namespace window {

GridWindow::GridWindow() : ScrolledWindow() {}

GridWindow::GridWindow(wxWindow *parent, int id, const wxPoint &pos,
                       const wxSize &size, long style,
                       const wxString &panel_name) {
  Create(parent, id, pos, size, style, panel_name);
}

bool GridWindow::Create(wxWindow *parent, int id, const wxPoint &pos,
                        const wxSize &size, long style,
                        const wxString &panel_name) {
  pos_ctrl_ = std::make_unique<position::GridCtrl>();
  bool ret = ScrolledWindow::Create(parent, id, pos, size, style, panel_name);
  Bind(wxEVT_KEY_DOWN, &GridWindow::OnKeyDown, this);
  return ret;
}

void GridWindow::AddCell(std::unique_ptr<GridCell> cell) {
  auto item = std::make_unique<GridItem>(std::move(cell));
  item->SetBorderSize(GetBorderSize());
  cell_vec_.push_back(std::move(item));
}

void GridWindow::AdjustCell() {
  std::vector<position::PositionItemRef<GridItem>> vec_ref;

  for (auto &it : cell_vec_) {
    vec_ref.push_back(*it);
  }

  std::vector<position::PositionItemRef<GridItem> *> vec_ptr =
      Vector::ConvertToPtr(vec_ref);
  position::PositionVector vec(vec_ptr.begin(), vec_ptr.end());

  pos_ctrl_->SetMinimumSize(GetTableSize());
  pos_ctrl_->CalculatePosition(vec);

  wxSize size = pos_ctrl_->GetSize(vec);
  SetVirtualSize(size);

  const position::PositionVectorConst const_vec(vec.begin(), vec.end());
  auto cell = pos_ctrl_->CountCell(const_vec);

  row_count_ = cell.row;
  col_count_ = cell.col;

  AdjustScrollbars();
}

std::vector<GridCell *> GridWindow::GetAllCell() {
  std::vector<GridCell *> ret;
  for (auto &it : cell_vec_) {
    ret.push_back(it->GetCell());
  }
  return ret;
}

void GridWindow::SetCellSize(const wxSize &size) {
  for (auto &it : cell_vec_) {
    it->GetCell()->SetSize(size);
  }
}

void GridWindow::ClearCell() { cell_vec_.clear(); }

wxSize GridWindow::GetBorderSize() const { return border_size_; }

void GridWindow::SetBorderSize(const wxSize &size) {
  for (auto &it : cell_vec_) {
    it->SetBorderSize(size);
  }

  border_size_ = size;
}

GridCell *GridWindow::GetCellWindow(size_t index) {
  if (IsExist(index)) return cell_vec_[index]->GetCell();

  return nullptr;
}

bool GridWindow::IsExist(size_t index) const {
  return Vector::IsExist(cell_vec_, index);
}

GridCellCoords GridWindow::IndexToCell(size_t index) const {
  if (index < 0) return GridCellCoords(-1, -1);

  int col = index / GetCols();
  int row = index - GetCols() * col;

  return GridCellCoords(row, col);
}

int GridWindow::CellToIndex(int row, int col, bool no_continous) const {
  size_t index = col * GetCols() + row;
  if (no_continous) {
    GridCellCoords cell = IndexToCell(index);
    if (cell.GetRow() != row || cell.GetCol() != col) return -1;
  }
  return index;
}

void GridWindow::SelectGridCursor(size_t index) {
  if (!IsExist(index)) return;

  if (IsExist(selected_index_)) {
    cell_vec_.at(selected_index_)->SetSelected(false);
  }

  cell_vec_.at(index)->SetSelected();
  selected_index_ = index;
}

void GridWindow::MakeCellVisible(size_t index) {
  if (!IsExist(index)) return;

  wxRect rect = cell_vec_.at(index)->GetRect();
  wxRect scroll_rect(GetViewStart(), GetClientSize());
  wxSize size = GetClientSize();
  wxPoint scroll_to = GetViewStart();

  if (rect.GetLeft() < scroll_rect.GetLeft()) {
    scroll_to.x = rect.GetLeft();
  }
  if (rect.GetRight() > scroll_rect.GetRight()) {
    scroll_to.x = rect.GetRight() - scroll_rect.GetWidth();
  }
  if (rect.GetTop() < scroll_rect.GetTop()) {
    scroll_to.y = rect.GetTop();
  }
  if ((rect.GetBottom() > scroll_rect.GetBottom())) {
    scroll_to.y = rect.GetBottom() - scroll_rect.GetHeight();
  }

  Scroll(scroll_to);
}

void GridWindow::GoToCell(size_t index) {
  SelectGridCursor(index);
  MakeCellVisible(index);
}

void GridWindow::OnDraw(wxDC &dc) {
  for (auto &it : cell_vec_) {
    if (it) it->Draw(dc);
  }
}

GridCellCoords GridWindow::CellFromSelected(wxDirection direction,
                                            bool no_continous) {
  GridCellCoords cell = IndexToCell(selected_index_);

  if (direction == wxUP) {
    cell.SetCol(cell.GetCol() - 1);
  } else if (direction == wxDOWN) {
    cell.SetCol(cell.GetCol() + 1);
  }

  if (direction == wxLEFT) {
    cell.SetRow(cell.GetRow() - 1);
  } else if (direction == wxRIGHT) {
    cell.SetRow(cell.GetRow() + 1);
  }

  if (!no_continous) return cell;

  size_t index = CellToIndex(cell, false);

  return IndexToCell(index);
}

void GridWindow::OnKeyDown(wxKeyEvent &event) {
  GridCellCoords cell = IndexToCell(selected_index_);

  wxDirection direction = dimension::GetDirection(event.GetKeyCode());

  if (!IsExist(selected_index_))
    cell = IndexToCell(0);
  else if (direction != wxALL) {
    cell = CellFromSelected(direction);
  }

  if (direction != wxALL && IsExist(cell)) {
    GoToCell(cell);
    Refresh();
    return;
  }
  event.Skip();
}

}  // namespace window

}  // namespace fmr
