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

wxBEGIN_EVENT_TABLE( Window, ScrolledWindow )
        EVT_COMMAND( LoadThread, EVT_COMMAND_THREAD_UPDATE, Window::OnThreadUpdate )
        EVT_COMMAND( LoadThread, EVT_COMMAND_THREAD_COMPLETED, Window::OnThreadUpdate )
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
}

bool Window::Destroy()
{
    wxCriticalSection lock;
    DeleteThread( load_thread_, lock );
    return wxWindow::Destroy();
}

wxThread *Window::GetThread( int id )
{
    switch ( id )
    {
        case LoadThread:
            return load_thread_;
    }
}

void Window::DoSetNull( int id )
{
    switch ( id )
    {
        case LoadThread:
            load_thread_ = NULL;
    }
}

void Window::Open( const wxString &name )
{
    handler_ = HandlerFactory::NewHandler( name ); 
    handler_->Open( name );
    handler_->Traverse(true);
    list_stream_ = handler_->GetChild();
    for ( auto &it : list_stream_ )
    {
        list_panel_.push_back( new ImageWindow( this, wxID_ANY ));
        list_panel_.back()->SetStream( &it );
    }

    load_thread_ = new Load( this, wxTHREAD_DETACHED, LoadThread );
    load_thread_->SetParameter( list_stream_ );
    load_thread_->Run();
}

void Window::OnThreadUpdate( wxCommandEvent &event )
{
    DoSetNull(event.GetId());
}


}; // namespace explorer

}; // namespace fmr