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

#include <fmr/window/grid_window.h>
#include <wx/dcclient.h>

namespace fmr
{

wxBEGIN_EVENT_TABLE( GridWindow, ScrolledWindow )
    EVT_KEY_DOWN( GridWindow::OnKeyDown )
    EVT_PAINT( GridWindow::OnPaint )
wxEND_EVENT_TABLE()

GridWindow::GridWindow(
        wxWindow *parent,
        wxWindowID id,
        const wxPoint &pos,
        const wxSize &size,
        long style,
        const wxString &name
    )
{
    Create( parent, id, pos, size, style, name );
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
    return ScrolledWindow::Create( parent, id, pos, size, style, name )
            && CreateGrid();
}

bool GridWindow::CreateGrid( int rows, int cols, const wxSize &gap )
{
    sizer_ = new wxFlexGridSizer( rows, cols, wxSize(10,10) );
    SetSizer( sizer_ );
    return sizer_;
}

void GridWindow::OnKeyDown( wxKeyEvent &event )
{

}

void GridWindow::OnPaint( wxPaintEvent &event )
{
}


} // namespace fmr
