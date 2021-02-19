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

#include "fmr/bitmap/position_ctrl.h"
namespace fmr {

namespace window {

bool GridWindow::Create(wxWindow *parent, int id, const wxPoint &pos,
                        const wxSize &size, long style,
                        const wxString &panel_name) {
  pos_ctrl_ = std::make_unique<bitmap::PositionCtrl>();
  return ScrolledWindow::Create(parent, id, pos, size, style, panel_name);
}

}  // namespace window

}  // namespace fmr
