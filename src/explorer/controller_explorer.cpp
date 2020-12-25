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

#include "explorer/controller_explorer.h"
#include "base/vector.h"

#include <wx/log.h>

namespace fmr
{

namespace explorer
{

wxBEGIN_EVENT_TABLE( Controller, ThreadController )
    EVT_STREAM( kFindThreadID, EVT_STREAM_FOUND, Controller::OnFound )
    EVT_COMMAND( kLoadThreadID, EVT_COMMAND_THREAD_UPDATE, Controller::OnUpdate )
wxEND_EVENT_TABLE()

Controller::Controller( wxWindow *parent )
{
    parent_ = parent;

    load_thread_ = new LoadThread( this, wxTHREAD_DETACHED, kLoadThreadID );
    if ( load_thread_->Run() != wxTHREAD_NO_ERROR )
        wxLogMessage( "Can't Create Thread" );
}

Controller::~Controller()
{
    DeleteThread( kLoadThreadID, g_sLock );
    DeleteThread( kFindThreadID, g_sLock );
}

void Controller::DoSetNull( int id )
{
    switch ( id )
    {
        case kLoadThreadID:
            load_thread_ = NULL;
            break;
        case kFindThreadID:
            find_thread_ = NULL;
            break;
    };
}

wxThread *Controller::GetThread( int id )
{
    switch ( id )
    {
        case kLoadThreadID:
            return load_thread_;
        case kFindThreadID:
            return find_thread_;
    }
    return NULL;
}

void Controller::SetParameter( std::vector<StreamBitmap> &list_stream )
{
    list_stream_ = list_stream;
}

void Controller::OnFound( StreamEvent &event )
{
    if ( Vector::IsExist(  list_stream_, event.GetIndex() ))
    {

        StreamBitmap stream = list_stream_[ event.GetIndex() ];
        stream.stream = event.GetStream();

        if ( load_thread_ )
            load_thread_->Push( stream );
    }
}

void Controller::OnUpdate( wxCommandEvent &event )
{
    Update( event.GetId() );
}

void Controller::SetThumbSize( const wxSize &size )
{
    thumb_size_ = size;
    if ( load_thread_ )
        load_thread_->SetSize( thumb_size_ );
}

void Controller::Load()
{
    DeleteThread( kFindThreadID, g_sLock );

    if( load_thread_ )
        load_thread_->Clear();

    find_thread_ = new FindThread( this, wxTHREAD_DETACHED, kFindThreadID );
    find_thread_->SetParameter( list_stream_ );
    find_thread_->Run();
}



};

}; // namespace fmr