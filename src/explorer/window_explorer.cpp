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
#include <wx/filename.h>

namespace fmr
{

wxDEFINE_EVENT( EVT_OPEN_FILE, wxCommandEvent );

namespace explorer
{


wxBEGIN_EVENT_TABLE( Window, wxGrid )
        EVT_COMMAND( kLoadThreadID, EVT_COMMAND_THREAD_UPDATE, Window::OnThreadUpdate )
        EVT_COMMAND( kLoadThreadID, EVT_COMMAND_THREAD_COMPLETED, Window::OnThreadUpdate )
        EVT_CHAR_HOOK( Window::OnGridEnter )
        EVT_GRID_SELECT_CELL( Window::OnGridSelect )
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
    SetCellHighlightColour( *wxWHITE );
    SetGridLineColour( *wxBLACK );
    CreateGrid( 0, 0 );
    grid_table_ = new wxGridStringTable();
    SetTable( grid_table_, true );
}

bool Window::Destroy()
{
    return wxWindow::Destroy();
}

bool Window::Open( std::shared_ptr<AbstractHandler> handler )
{
    if ( !handler )
        return false;

    handler->Traverse( false );

    int column = 5;
    int row = ceil(double(handler->Size()) / double(column));

    Clear();

    int size = GetSize().GetWidth() / column;
    SetDefaultRowSize( size );
    SetDefaultColSize( size );

    wxSize best_bitmap_size = ImageWindow::GetBestBitmapSize(
        wxSize( size, size )
    );

    controller_.SetThumbSize( best_bitmap_size );


    GetTable()->AppendRows( row );
    GetTable()->AppendCols( column );


    std::vector<StreamBitmap> list_item;
    list_item.assign( handler->Size(), StreamBitmap() );

    size_t idx = 0;
    for ( auto &it : handler->GetChild() )
    {
        list_item.at(idx).stream =
            std::shared_ptr<SStream>( new SStream( std::move(it) ) );
        list_item.at(idx).bitmap =
            std::shared_ptr<SBitmap>( new SBitmap() );
        idx++;
    }

    SetGridCursor(0,0);
    handler->Clear();

    int cur_row = 0;
    int cur_col = 0;
    for ( auto &it : list_item )
    {
        auto renderer =  new ImageWindow();
        renderer->SetBitmap( it.bitmap );
        renderer->SetStream( it.stream );
        SetCellRenderer( cur_row, cur_col, renderer );

        list_cell_pos_.push_back( wxGridCellCoords( cur_row, cur_col ) );
        list_renderer_.push_back( renderer );

        cur_col++;
        if ( cur_col >= column )
        {
            cur_row += 1;
            cur_col = 0;
        }
    }


    list_item_ = list_item;
    controller_.SetParameter( list_item );
    controller_.Load();
    Refresh();
    return true;
}

bool Window::Open( const wxString &name )
{
    if ( name.IsEmpty() )
        return false;

    auto handler = std::shared_ptr<AbstractHandler>(
        HandlerFactory::NewHandler( name )
    );

    if ( Open( handler ) )
    {
        handler_ = handler;
        return true;
    }

    return false;
}

void Window::Clear()
{
    list_cell_pos_.clear();
    list_item_.clear();
    list_renderer_.clear();
    controller_.Clear();
    handler_.reset();
    if ( grid_table_->GetRowsCount() > 0 )
        grid_table_->DeleteRows( 0, grid_table_->GetRowsCount() );
    if ( grid_table_->GetColsCount() > 0 )
        grid_table_->DeleteCols( 0, grid_table_->GetColsCount() );
}

void Window::OnThreadUpdate( wxCommandEvent &event )
{
    Refresh();
}

void Window::OnGridEnter( wxKeyEvent &event )
{
    int key_code = event.GetKeyCode();
    if ( key_code == WXK_RETURN || key_code == WXK_NUMPAD_ENTER )
    {
        size_t idx = 0;
        for( const auto &it : list_cell_pos_ )
        {
            if ( it == selected_cell_ )
            {
                wxString path = list_item_.at(idx).stream->GetName();
                auto handler = std::shared_ptr<AbstractHandler>(
                    HandlerFactory::NewHandler( path )
                );

                handler->Traverse( true );

                if ( wxFileName::DirExists( path ) )
                    for ( const auto &it : handler->GetChild() )
                        if ( ! wxImage::CanRead( *it.GetStream() ) )
                            return void( Open( path ) );

                wxCommandEvent *open_event = new wxCommandEvent(
                        EVT_OPEN_FILE,
                        GetId()
                );

                open_event->SetString( path );

                wxQueueEvent(
                    GetParent(),
                    open_event
                );
            }
            idx++;
        }
        return;
    }

    if ( key_code == WXK_BACK )
    {
        if ( handler_->GetParent() )
        {
            wxString curr_name = handler_->GetName();
            wxString name = handler_->GetParent()->GetName();
            Open( name );
            Select( curr_name );
        }
        return;
    }
    event.Skip();
}

void Window::Select( const wxString &name )
{
    size_t idx = 0;
    for ( const auto &it : list_cell_pos_ )
    {
        auto renderer = list_renderer_[idx];
        if ( renderer->GetStream() )
            if ( renderer->GetStream()->GetName() == name )
                return GoToCell( it );

        idx++;
    }
}

void Window::OnGridSelect( wxGridEvent &event )
{
    wxGridCellCoords eve( event.GetRow(), event.GetCol() );

    bool is_change = false;
    for ( const auto &it : list_cell_pos_ )
        if ( event.GetRow() == it.GetRow() && event.GetCol() == it.GetCol())
            is_change = true;

    if ( is_change )
        selected_cell_.Set( event.GetRow(), event.GetCol() );
    else
        event.Veto();

    Refresh();
}


}; // namespace explorer

}; // namespace fmr