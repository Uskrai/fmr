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

#include <fmr/bitmap/inc.h>
#include <fmr/window/scrolledwindow.h>

#include <memory>

namespace fmr {

namespace window {

class GridWindow : ScrolledWindow {
 private:
  std::unique_ptr<bitmap::PositionCtrl> pos_ctrl_;

 public:
  GridWindow() : ScrolledWindow() {}
  GridWindow(wxWindow *parent, int id, const wxPoint &pos = wxDefaultPosition,
             const wxSize &size = wxDefaultSize, long style = 0,
             const wxString &panel_name = wxPanelNameStr) {
    Create(parent, id, pos, size, style, panel_name);
  };

  bool Create(wxWindow *parent, int id, const wxPoint &pos = wxDefaultPosition,
              const wxSize &size = wxDefaultSize, long style = 0,
              const wxString &panel_name = wxPanelNameStr);
};

}  // namespace window

}  // namespace fmr
