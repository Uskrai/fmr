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

#ifndef FMR_EXPLORER_WINDOW
#define FMR_EXPLORER_WINDOW

#include <fmr/common/path.h>
#include <fmr/explorer/common.h>
#include <fmr/explorer/controller_explorer.h>
#include <fmr/explorer/image_window_explorer.h>
#include <fmr/handler/abstract_handler.h>
#include <fmr/handler/struct_stream.h>
#include <fmr/window/flex_grid_window.h>

#include <map>
#include <memory>

class wxPanel;

namespace fmr {

wxDECLARE_EVENT(EVT_OPEN_FILE, wxCommandEvent);

namespace explorer {

class Window : public FlexGridWindow {
 protected:
  Controller controller_ = Controller(this);
  std::map<wxWindow *, StreamBitmap> map_window_;
  std::shared_ptr<AbstractOpenableHandler> handler_;
  std::vector<SBitmap> list_bitmap_;
  std::vector<StreamBitmap> list_item_;
  std::vector<ImageWindow *> list_renderer_;

 public:
  Window(wxWindow *parent, const wxWindowID &id = wxID_ANY,
         const wxPoint &pos = wxDefaultPosition,
         const wxSize &size = wxDefaultSize,
         const long &style = wxTAB_TRAVERSAL,
         const wxString &name = wxPanelNameStr);

  bool Open(std::shared_ptr<AbstractOpenableHandler> handler);
  bool Open(const std::string &name);

  bool OpenCell(int index);
  bool OpenCell(int row, int col) { return OpenCell(CellToIndex(row, col)); }
  bool OpenCell(GridCellCoords cell) {
    return OpenCell(cell.GetRow(), cell.GetCol());
  }

  bool OpenParent();
  bool OpenParent(const std::string &path);

  void Select(std::string name);

  void Clear();
  bool Destroy();

 protected:
  void BindEvent();
  // wxGridTableBase *grid_table_;

  // std::vector<wxGridCellCoords> list_cell_pos_;

  void OnThreadUpdate(wxThreadEvent &event);
  void OnKeyDown(wxKeyEvent &event);
  // void OnGridSelect( wxGridEvent &event );

  // wxGridCellCoords selected_cell_;
};

};  // namespace explorer

};  // namespace fmr
#endif
