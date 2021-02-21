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

#include <fmr/position/inc.h>
#include <fmr/window/scrolledwindow.h>
#include <wx/grid.h>

#include <memory>

namespace fmr {

namespace window {

typedef wxGridCellCoords GridCellCoords;
class GridItem;
class GridCell;

class GridWindow : public ScrolledWindow {
 private:
  std::unique_ptr<position::GridCtrl> pos_ctrl_;
  std::vector<std::unique_ptr<GridItem>> cell_vec_;
  size_t selected_index_ = -1;
  wxSize border_size_, table_size_;
  int cell_highlight_width_ = 0;
  size_t row_count_ = 0, col_count_ = 0;

 public:
  GridWindow();
  GridWindow(wxWindow *parent, int id, const wxPoint &pos = wxDefaultPosition,
             const wxSize &size = wxDefaultSize, long style = 0,
             const wxString &panel_name = wxPanelNameStr);

  bool Create(wxWindow *parent, int id, const wxPoint &pos = wxDefaultPosition,
              const wxSize &size = wxDefaultSize, long style = 0,
              const wxString &panel_name = wxPanelNameStr);

  /**
   * @brief: Add Cell to the grid
   */
  void AddCell(std::unique_ptr<GridCell> cell);
  void AdjustCell();
  void ClearCell();
  void SetCellSize(const wxSize &size);

  std::vector<GridCell *> GetAllCell();

  GridCell *GetSelectedCell() { return GetCellWindow(selected_index_); }
  size_t GetSelectedIndex() { return selected_index_; }

  position::GridCtrl *GetPosCtrl() { return pos_ctrl_.get(); }

  GridCell *GetCellWindow(size_t index);
  GridCell *GetCellWindow(GridCellCoords cell) {
    return GetCellWindow(cell.GetRow(), cell.GetCol());
  }
  GridCell *GetCellWindow(int row, int col) {
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

  GridCellCoords CellFromSelected(wxDirection direction,
                                  bool no_continous = false);

  wxSize GetBorderSize() const;
  void SetBorderSize(const wxSize &size);

  wxSize GetTableSize() const { return table_size_; }
  void SetTableSize(const wxSize &size) { table_size_ = size; }

  int GetCellHighlightPenWidth() const { return cell_highlight_width_; }

  int GetCols() const { return col_count_; }
  int GetRows() const { return row_count_; }

 protected:
  void OnDraw(wxDC &dc) override;
  void OnKeyDown(wxKeyEvent &event);
};

}  // namespace window

}  // namespace fmr
