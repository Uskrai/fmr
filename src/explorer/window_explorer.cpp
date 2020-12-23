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

#include "explorer/window_explorer.h"

namespace fmr
{

namespace explorer
{

wxBEGIN_EVENT_TABLE( Window, wxGrid )
        EVT_COMMAND( kLoadThreadID, EVT_COMMAND_THREAD_UPDATE, Window::OnThreadUpdate )
        EVT_COMMAND( kLoadThreadID, EVT_COMMAND_THREAD_COMPLETED, Window::OnThreadUpdate )
wxEND_EVENT_TABLE()

Window::Window( 
        wxWindow *parent, 
        const wxWindowID &id,
        const wxPoint &pos,
        const wxSize &size,
        const long &style,
        const wxString &name
)   : wxGrid( parent, id, pos, size, style, name)
{
    HideColLabels();
    HideRowLabels();
    DisableCellEditControl();
    EnableEditing( false );
}

bool Window::Destroy()
{
    return wxWindow::Destroy();
}

void Window::Open( const wxString &name )
{
    if ( name.IsEmpty() )
        return;

    handler_ = std::shared_ptr<AbstractHandler>(
        HandlerFactory::NewHandler( name )
    );

    handler_->Traverse(true);

    wxSize sz = GetSize();

    int column = sz.GetWidth() / 300;
    int row = ceil(double(handler_->Size()) / double(column));

    SetDefaultColSize( 300 );
    SetDefaultRowSize( 300 );
    CreateGrid( row, column );

    list_bitmap_.clear();
    list_bitmap_.assign( handler_->Size(), SBitmap() );

    std::vector<StreamBitmap> list_item;
    list_item.assign( handler_->Size(), StreamBitmap() );
    size_t idx = 0;
    for ( auto &it : handler_->GetChild() )
    {
        list_item.at(idx).stream =
            new SStream( std::move(it) );
        list_item.at(idx).bitmap =
            new SBitmap();
        idx++;
    }

    handler_->Clear();

    int cur_row = 0;
    int cur_col = 0;
    for ( auto &it : list_item )
    {
        auto renderer =  new ImageWindow();
        renderer->SetBitmap( it.bitmap );
        renderer->SetStream( it.stream );
        SetCellRenderer( cur_row, cur_col, renderer );

        list_cell_pos_.push_back( wxGridCellCoords( cur_row, cur_col ) );
        cur_col++;
        if ( cur_col >= column )
        {
            cur_row += 1;
            cur_col = 0;
        }
    }

    controller_.SetParameter( list_item );
    controller_.Load();
}

void Window::OnThreadUpdate( wxCommandEvent &event )
{
}


}; // namespace explorer

}; // namespace fmr