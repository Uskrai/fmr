/*
 *  Copyright (c) 2020-2021 Uskrai
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

#include <fmr/common/dimension.h>
#include <fmr/window/flex_grid_window.h>
#include <wx/dcclient.h>

namespace fmr {

FlexGridWindow::FlexGridWindow(wxWindow *parent, wxWindowID id,
                               const wxPoint &pos, const wxSize &size,
                               long style, const wxString &name) {
  Create(parent, id, pos, size, style, name);
}

bool FlexGridWindow::Create(wxWindow *parent, wxWindowID id, const wxPoint &pos,
                            const wxSize &size, long style,
                            const wxString &name) {
  bool ret = ScrolledWindow::Create(parent, id, pos, size, style, name);
  BindEvent();
  return ret && CreateGrid();
}

void FlexGridWindow::BindEvent() {
  Bind(wxEVT_KEY_DOWN, &FlexGridWindow::OnKeyDown, this);
}

bool FlexGridWindow::CreateGrid(int rows, int cols, const wxSize &gap) {
  sizer_ = new wxFlexGridSizer(rows, cols, gap);
  SetSizer(sizer_);
  for (auto &it : vec_cells_) sizer_->Add(it);

  return sizer_;
}

void FlexGridWindow::Add(FlexGridCellWindow *window) {
  window->SetBorderWidth(GetCellBorderWidth());
  window->SetHighlightPenWidth(GetCellHighlightPenWidth());
  sizer_->Add(window);
  vec_cells_.push_back(window);
}

void FlexGridWindow::Add(wxWindow *cell) {
  FlexGridCellWindow *window;
  window = new FlexGridCellWindow(this, wxID_ANY);

  cell->Reparent(window);
  window->SetCellWindow(cell);

  Add(window);
}

void FlexGridWindow::ClearCell(bool delete_child_window) {
  if (sizer_) sizer_->Clear(delete_child_window);

  vec_cells_.clear();
}

FlexGridCellWindow *FlexGridWindow::GetCellWindow(size_t index) {
  if (IsExist(index)) return vec_cells_[index];

  return nullptr;
}

bool FlexGridWindow::IsExist(size_t index) const {
  return Vector::IsExist(vec_cells_, index);
}

GridCellCoords FlexGridWindow::IndexToCell(size_t index) const {
  if (index < 0) return GridCellCoords(-1, -1);

  int col = index / GetCols();
  int row = index - GetCols() * col;

  return GridCellCoords(row, col);
}

int FlexGridWindow::CellToIndex(int row, int col, bool no_continous) const {
  size_t index = col * GetCols() + row;
  if (no_continous) {
    GridCellCoords cell = IndexToCell(index);
    if (cell.GetRow() != row || cell.GetCol() != col) return -1;
  }
  return index;
}

void FlexGridWindow::SelectGridCursor(size_t index) {
  if (!IsExist(index)) return;

  if (IsExist(selected_index_))
    vec_cells_.at(selected_index_)->SetSelected(false);

  vec_cells_.at(index)->SetSelected();
  selected_index_ = index;
}

void FlexGridWindow::MakeCellVisible(size_t index) {
  if (!IsExist(index)) return;

  wxRect rect = vec_cells_.at(index)->GetRect();
  wxSize size = GetClientSize();
  wxPoint scroll_to = GetViewStart();

  if (rect.GetLeft() < 0)
    LineUp(wxHORIZONTAL, -rect.GetLeft());
  else if (rect.GetRight() > size.GetWidth())
    LineDown(wxHORIZONTAL, rect.GetRight() - size.GetWidth());

  if (rect.GetTop() < 0)
    LineUp(wxVERTICAL, -rect.GetTop());
  else if (rect.GetBottom() > size.GetHeight())
    LineDown(wxVERTICAL, rect.GetBottom() - size.GetHeight());

  Scroll(scroll_to);
}

void FlexGridWindow::GoToCell(size_t index) {
  SelectGridCursor(index);
  MakeCellVisible(index);
}

void FlexGridWindow::SetCellBorderWidth(int border) {
  cell_border_width_ = border;

  for (auto &it : vec_cells_)
    if (it) it->SetBorderWidth(border);

  Refresh();
}

void FlexGridWindow::SetCellHighlightPenWidth(int width) {
  cell_highlight_width_ = width;

  for (auto &it : vec_cells_)
    if (it) it->SetHighlightPenWidth(width);

  Refresh();
}

GridCellCoords FlexGridWindow::CellFromSelected(wxDirection direction,
                                                bool no_continous) {
  GridCellCoords cell = IndexToCell(selected_index_);
  if (direction == wxUP)
    cell.SetCol(cell.GetCol() - 1);
  else if (direction == wxDOWN)
    cell.SetCol(cell.GetCol() + 1);

  if (direction == wxLEFT)
    cell.SetRow(cell.GetRow() - 1);
  else if (direction == wxRIGHT)
    cell.SetRow(cell.GetRow() + 1);

  if (!no_continous) return cell;

  size_t index = CellToIndex(cell, false);

  return IndexToCell(index);
}

void FlexGridWindow::OnKeyDown(wxKeyEvent &event) {
  GridCellCoords cell = IndexToCell(selected_index_);

  wxDirection direction = dimension::GetDirection(event.GetKeyCode());

  if (!IsExist(selected_index_))
    cell = IndexToCell(0);
  else
    cell = CellFromSelected(direction);

  if (direction != wxALL && IsExist(cell)) {
    GoToCell(cell);
    Refresh();
    return;
  }
  event.Skip();
}

}  // namespace fmr
