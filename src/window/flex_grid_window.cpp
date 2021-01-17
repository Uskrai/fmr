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

#include <fmr/window/flex_grid_window.h>
#include <wx/dcclient.h>

namespace fmr
{

wxBEGIN_EVENT_TABLE( FlexGridWindow, ScrolledWindow )
    EVT_KEY_DOWN( FlexGridWindow::OnKeyDown )
wxEND_EVENT_TABLE()

FlexGridWindow::FlexGridWindow(
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

bool FlexGridWindow::Create(
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

bool FlexGridWindow::CreateGrid( int rows, int cols, const wxSize &gap )
{
    sizer_ = new wxFlexGridSizer( rows, cols, gap );
    SetSizer( sizer_ );
    return sizer_;
}

void FlexGridWindow::Add( FlexGridCellWindow *window )
{
    window->SetBorderWidth( GetCellBorderWidth() );
    window->SetHighlightPenWidth( GetCellHighlightPenWidth() );
    sizer_->Add( window );
}

void FlexGridWindow::Add( wxWindow *cell )
{
    FlexGridCellWindow *window;
    window = new FlexGridCellWindow( this, wxID_ANY );

    cell->Reparent( window );
    window->SetCellWindow( cell );

    Add( window );
}

void FlexGridWindow::OnKeyDown( wxKeyEvent &event )
{
    event.Skip();
}

void FlexGridWindow::SetCellBorderWidth( int border )
{
    cell_border_width_ = border;

    for ( auto &it : vec_cells_ )
        if ( it )
            it->SetBorderWidth( border );

    Refresh();
}

void FlexGridWindow::SetCellHighlightPenWidth( int width )
{
    cell_highlight_width_ = width;

    for ( auto &it : vec_cells_ )
        if ( it )
            it->SetHighlightPenWidth( width );

    Refresh();
}


} // namespace fmr

