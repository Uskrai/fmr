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

#include <fmr/explorer/controller_explorer.h>
#include <fmr/common/vector.h>

#include <wx/log.h>

namespace fmr
{

namespace explorer
{

wxBEGIN_EVENT_TABLE( Controller, ThreadController )
    EVT_STREAM_BITMAP( kFindThreadID, EVT_STREAM_FOUND, Controller::OnFound )
    EVT_STREAM_BITMAP( kLoadThreadID, EVT_BITMAP_LOADED, Controller::OnLoaded )
    EVT_COMMAND( kLoadThreadID, EVT_COMMAND_THREAD_UPDATE, Controller::OnUpdate )
    EVT_COMMAND( kFindThreadID, EVT_COMMAND_THREAD_COMPLETED, Controller::OnFindCompleted )
wxEND_EVENT_TABLE()

Controller::Controller( wxWindow *parent )
{
    parent_ = parent;

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

void Controller::Clear()
{
    Delete( kLoadThreadID, g_sLock );
    Delete( kFindThreadID, g_sLock );
}

void Controller::SetParameter( std::vector<StreamBitmap> &list_stream )
{
    list_stream_ = list_stream;
}

void Controller::OnFound( StreamBitmapEvent &event )
{
    if ( load_thread_ )
        load_thread_->Push( event.GetStreamBitmap() );
}

void Controller::OnLoaded( StreamBitmapEvent &event )
{
}

void Controller::OnUpdate( wxCommandEvent &event )
{
    Update( event.GetId() );
}

void Controller::OnFindCompleted( wxCommandEvent &event )
{
    if ( load_thread_ )
        load_thread_->DeleteOnEmptyQueue();
}

void Controller::SetThumbSize( const wxSize &size )
{
    thumb_size_ = size;
}

void Controller::Load()
{
    DeleteThread( kFindThreadID, g_sLock );
    DeleteThread( kLoadThreadID, g_sLock );


    find_thread_ = new FindThread( this, wxTHREAD_DETACHED, kFindThreadID );
    find_thread_->DisableEventOnDestroy();
    find_thread_->SetParameter( list_stream_ );

    load_thread_ = new LoadThread( this, wxTHREAD_DETACHED, kLoadThreadID );
    load_thread_->SetSize( thumb_size_ );

    load_thread_->Run();
    find_thread_->Run();
}



};

}; // namespace fmr
