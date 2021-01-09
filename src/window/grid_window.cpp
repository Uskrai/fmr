/*
 *  Copyright (c) 2020 Uskrai
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

#include "window/grid_window.h"

namespace fmr
{

GridWindow::GridWindow(
    wxWindow *parent,
    wxWindowID id,
    const wxPoint &pos,
    const wxSize &size,
    long style,
    const wxString &name
)   : wxWindow( parent, id, pos, size, 0, name )
{
    CreateGrid();
}

bool GridWindow::Create(
    wxWindow *parent,
    wxWindowID id,
    const wxPoint &pos,
    const wxSize &size,
    long style,
    const wxString &name
)
{
    return Create( parent, id, pos, size, style, name )
            && CreateGrid();
}

bool GridWindow::CreateGrid( int rows, int cols, const wxSize &gap )
{
    sizer_ = new wxGridSizer( rows, cols, gap );
    SetSizer( sizer_ );
    return sizer_;
}

} // namespace fmr