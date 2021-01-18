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

#include <fmr/explorer/window_explorer.h>
#include <wx/filename.h>

namespace fmr
{

wxDEFINE_EVENT( EVT_OPEN_FILE, wxCommandEvent );

namespace explorer
{


wxBEGIN_EVENT_TABLE( Window, FlexGridWindow )
        EVT_COMMAND( kLoadThreadID, EVT_COMMAND_THREAD_UPDATE, Window::OnThreadUpdate )
        EVT_COMMAND( kLoadThreadID, EVT_COMMAND_THREAD_COMPLETED, Window::OnThreadUpdate )
        EVT_CHAR_HOOK( Window::OnGridEnter )
wxEND_EVENT_TABLE()

Window::Window(
        wxWindow *parent,
        const wxWindowID &id,
        const wxPoint &pos,
        const wxSize &size,
        const long &style,
        const wxString &name
)   : FlexGridWindow( parent, id, pos, size, style, name)
{
    // HideColLabels();
    // HideRowLabels();
    // DisableCellEditControl();
    // EnableEditing( false );
    // SetCellHighlightColour( *wxWHITE );
    // SetGridLineColour( *wxBLACK );
    CreateGrid( 0, 0 );
    // grid_table_ = new wxGridStringTable();
    // SetTable( grid_table_, true );
}

bool Window::Destroy()
{
    return wxWindow::Destroy();
}

bool Window::Open( std::shared_ptr<AbstractOpenableHandler> handler )
{
    if ( !handler )
        return false;

    handler->Traverse( false );

    int column = 5;
    int row = ceil(double(handler->Size()) / double(column) );

    Clear();

    // 11 * 2 * column is an offset from border
    // and gap multiplied by column
    int both_size = ( (GetSize().GetWidth() - (11 * 2 * column) ) / column);

    wxSize child_size = wxSize( both_size, both_size );
    wxSize best_bitmap_size = ImageWindow::GetBestBitmapSize(
        child_size
    );

    controller_.SetThumbSize( best_bitmap_size );


    CreateGrid( row, column, wxSize( 1, 1 ) );

    std::vector<StreamBitmap> list_item;
    list_item.assign( handler->Size(), StreamBitmap() );

    size_t idx = 0;
    for ( auto &it : handler->GetChild() )
    {
        auto &item = list_item.at(idx);
        item.stream = std::shared_ptr<SStream>( new SStream( it ) );
        item.bitmap = std::shared_ptr<SBitmap>( new SBitmap() );

        ImageWindow *image_cell_window = new ImageWindow(
                    this,
                    wxID_ANY,
                    wxDefaultPosition,
                    child_size
                );

        image_cell_window->SetBitmap( item.bitmap );
        image_cell_window->SetStream( item.stream );
        image_cell_window->SetBackgroundColour( *wxBLACK );

        Add( image_cell_window );

        map_window_.insert( std::make_pair( image_cell_window, item ) );

        idx++;
    }

    SetCellBorderWidth( 10 );
    SetCellHighlightPenWidth( 3 );

    list_item_ = list_item;
    controller_.SetParameter( list_item );
    controller_.Load();
    Refresh();
    handler_ = handler;
    Layout();
    FitInside();
    return true;
}

bool Window::Open( const std::string &name )
{
    if ( name.empty() )
        return false;

    auto handler = std::shared_ptr<AbstractOpenableHandler>(
        HandlerFactory::NewOpenableHandler( name )
    );

    return Open( handler );
}

void Window::Clear()
{
    ClearCell( true );
    ResetCellPosition();
    map_window_.clear();
    list_item_.clear();
    list_renderer_.clear();
    controller_.Clear();
    handler_.reset();
}

void Window::OnThreadUpdate( wxCommandEvent &event )
{
    Refresh();
}

void Window::OnGridEnter( wxKeyEvent &event )
{
    // int key_code = event.GetKeyCode();
    // if ( key_code == WXK_RETURN || key_code == WXK_NUMPAD_ENTER )
    // {
    //     size_t idx = 0;
    //     for( const auto &it : list_cell_pos_ )
    //     {
    //         if ( it == selected_cell_ )
    //         {
    //             auto &stream = list_item_.at(idx).stream;

    //             if ( !stream )
    //                 return;

    //             std::string path = handler_->GetItemPath( *stream );

    //             auto handler = std::shared_ptr<AbstractHandler>(
    //                 HandlerFactory::NewHandler( path )
    //             );

    //             if ( !handler )
    //                 return;

    //             handler->Traverse( true );

    //             if ( wxFileName::DirExists( path ) )
    //                 for ( const auto &it : handler->GetChild() )
    //                     if ( ! wxImage::CanRead( *it.GetStream() ) )
    //                         return void( Open( path ) );

    //             wxCommandEvent *open_event = new wxCommandEvent(
    //                     EVT_OPEN_FILE,
    //                     GetId()
    //             );

    //             open_event->SetString( path );

    //             wxQueueEvent(
    //                 GetParent(),
    //                 open_event
    //             );
    //         }
    //         idx++;
    //     }
    //     return;
    // }

    // if ( key_code == WXK_BACK )
    // {
    //     if ( handler_->GetParent() )
    //     {
    //         std::string curr_name = handler_->GetName();
    //         std::string name = handler_->GetParent()->GetName();
    //         std::shared_ptr<AbstractOpenableHandler> handler(
    //             HandlerFactory::NewOpenableHandler( name )
    //         );

    //         Open( handler );

    //         size_t idx = handler->Index( curr_name );
    //         if ( handler->IsExist( idx ) )
    //             Select( handler->Item( idx ).GetName() );
    //     }
    //     return;
    // }
    event.Skip();
}

void Window::Select( std::string name )
{
    name = Path::GetName( name );

    // size_t idx = 0;
    // for ( const auto &it : list_cell_pos_ )
    // {
    //     // auto renderer = list_renderer_[idx];
    //     // if ( renderer->GetStream() )
    //     //     if ( renderer->GetStream()->GetName() == name )
    //     //         return GoToCell( it );

    //     idx++;
    // }
}

// void Window::OnGridSelect( wxGridEvent &event )
// {
//     wxGridCellCoords eve( event.GetRow(), event.GetCol() );

//     bool is_change = false;
//     for ( const auto &it : list_cell_pos_ )
//         if ( event.GetRow() == it.GetRow() && event.GetCol() == it.GetCol())
//             is_change = true;

//     if ( is_change )
//         selected_cell_.Set( event.GetRow(), event.GetCol() );
//     else
//         event.Veto();

//     Refresh();
// }


}; // namespace explorer

}; // namespace fmr

