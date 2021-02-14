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

#include <fmr/bitmap/rescale_loader.h>
#include <fmr/common/path.h>
#include <fmr/explorer/common.h>
#include <fmr/explorer/image_window_explorer.h>
#include <fmr/handler/abstract_handler.h>
#include <fmr/handler/struct_stream.h>
#include <fmr/window/flex_grid_window.h>

#include <map>
#include <memory>

#include "fmr/handler/handler_factory.h"

class wxPanel;

namespace fmr {

wxDECLARE_EVENT(EVT_OPEN_FILE, StreamEvent);

namespace explorer {

enum ControllerId {
  kLoaderId = wxID_HIGHEST + 1600
  //
};

class Window : public FlexGridWindow {
 protected:
  bitmap::RescaleLoader loader_ = bitmap::RescaleLoader(this, kLoaderId);
  std::unordered_map<const SStream *, ImageWindow *> map_window_;
  std::shared_ptr<AbstractOpenableHandler> handler_;
  std::vector<std::unique_ptr<SStream>> list_stream_;
  std::unique_ptr<bitmap::Rescaler> rescaler_ = nullptr;

 public:
  Window(wxWindow *parent, const wxWindowID &id = wxID_ANY,
         const wxPoint &pos = wxDefaultPosition,
         const wxSize &size = wxDefaultSize, const long &style = wxWANTS_CHARS,
         const wxString &name = wxPanelNameStr);
  ~Window();

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

  void OnThreadUpdate(wxThreadEvent &event);
  void OnKeyDown(wxKeyEvent &event);
  void OnImageLoaded(bitmap::ImageLoadEvent &event);
};

};  // namespace explorer

};  // namespace fmr
#endif
