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

#ifndef FMR_WINDOW_FMR_GRID_WINDOW
#define FMR_WINDOW_FMR_GRID_WINDOW

#include <fmr/common/vector.h>
#include <fmr/window/flex_grid_cell_window.h>
#include <fmr/window/scrolledwindow.h>
#include <wx/grid.h>
#include <wx/sizer.h>

#include <memory>

namespace fmr {

typedef wxGridCellCoords GridCellCoords;

class FlexGridWindow : public ScrolledWindow {
 protected:
  wxGridSizer *sizer_ = nullptr;
  wxVector<FlexGridCellWindow *> vec_cells_;
  size_t selected_index_ = -1;
  int cell_border_width_ = 0, cell_highlight_width_ = 0;

 public:
  FlexGridWindow(wxWindow *parent, wxWindowID id,
                 const wxPoint &pos = wxDefaultPosition,
                 const wxSize &size = wxDefaultSize, long style = 0,
                 const wxString &name = wxPanelNameStr);

  bool Create(wxWindow *parent, wxWindowID id,
              const wxPoint &pos = wxDefaultPosition,
              const wxSize &size = wxDefaultSize, long style = 0,
              const wxString &name = wxPanelNameStr);

  void SetCellBorderWidth(int size);
  void SetCellHighlightPenWidth(int width);

  bool CreateGrid(int rows = 0, int cols = 0,
                  const wxSize &gap = wxDefaultSize);

  void Add(FlexGridCellWindow *window);
  void Add(wxWindow *window_cell);

  void ClearCell(bool delete_child_window);

  FlexGridCellWindow *GetSelectedCell() {
    return GetCellWindow(selected_index_);
  }

  FlexGridCellWindow *GetCellWindow(size_t index);
  FlexGridCellWindow *GetCellWindow(GridCellCoords cell) {
    return GetCellWindow(cell.GetRow(), cell.GetCol());
  }
  FlexGridCellWindow *GetCellWindow(int row, int col) {
    return GetCellWindow(CellToIndex(row, col));
  }

  bool IsExist(size_t index) const;
  bool IsExist(GridCellCoords cell) const {
    return IsExist(cell.GetRow(), cell.GetCol());
  }
  bool IsExist(int row, int col) const {
    return IsExist(CellToIndex(row, col));
  }

  void SelectGridCursor(size_t index);
  void SelectGridCursor(GridCellCoords cell) {
    return SelectGridCursor(cell.GetRow(), cell.GetCol());
  }
  void SelectGridCursor(int row, int col) {
    return SelectGridCursor(CellToIndex(row, col));
  }

  void ResetCellPosition() { selected_index_ = -1; }

  void GoToCell(size_t index);
  void GoToCell(GridCellCoords cell) {
    return GoToCell(cell.GetRow(), cell.GetCol());
  }
  void GoToCell(int row, int col) { return GoToCell(CellToIndex(row, col)); }

  void MakeCellVisible(size_t index);
  void MakeCellVisible(GridCellCoords cell) {
    return MakeCellVisible(cell.GetRow(), cell.GetCol());
  }
  void MakeCellVisible(int row, int col) {
    return MakeCellVisible(CellToIndex(row, col));
  }

  GridCellCoords IndexToCell(size_t index) const;
  int CellToIndex(GridCellCoords cell, bool no_continous = false) const {
    return CellToIndex(cell.GetRow(), cell.GetCol(), no_continous);
  }
  int CellToIndex(int row, int col, bool no_continous = false) const;

  int GetCellBorderWidth() const { return cell_border_width_; }

  int GetCellHighlightPenWidth() const { return cell_highlight_width_; }

  int GetRows() const { return sizer_->GetRows(); }
  int GetCols() const { return sizer_->GetCols(); }

  GridCellCoords CellFromSelected(wxDirection direction,
                                  bool no_continous = false);

 private:
  void BindEvent();

  void OnKeyDown(wxKeyEvent &event);
};

};  // namespace fmr

#endif
