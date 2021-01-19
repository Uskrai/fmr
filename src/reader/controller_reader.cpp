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

#include <fmr/reader/controller_reader.h>
#include <fmr/handler/handler_factory.h>

namespace fmr
{

namespace reader
{

Controller::Controller( wxWindow *parent )
{
    parent_ = parent;
}
Controller::~Controller()
{
    DeleteThread( kLoadThreadID, g_sLock );
    DeleteThread( kZoomThreadID, g_sLock );
}

ControllerExit Controller::Open( const std::string &path )
{
    if ( path.empty() )
        return kControllerPathEmpty;
    
    if ( ! bitmap_ )
        return kControllerBitmapEmpty;
    
    auto opened_handler = std::shared_ptr<AbstractHandler>(
        HandlerFactory::NewHandler( path )
    );

    if ( ! opened_handler  )
        return kControllerHandlerNotFound;

    opened_handler->Traverse( true );
    if ( opened_handler->GetParent() )
        opened_handler->GetParent()->Traverse();

    if ( opened_handler->Size() == 0 )
        return kControllerFolderEmpty;

    size_t size = opened_handler->Size();
    bitmap_->Resize( size );
    bitmap_->GetAll().assign(
        size,
        SBitmap()
    );

    size_t idx = opened_handler->Index( path );
    LoadThread::LoadImage( bitmap_, opened_handler, idx, g_sLock );
    Update( kLoadThreadID );
    
    auto thread = new LoadThread( this, wxTHREAD_DETACHED, kLoadThreadID );
    thread->SetParameter( opened_handler, bitmap_, idx );
    
    if ( thread->Run() != wxTHREAD_NO_ERROR )
    {
        thread->SetId( -1 );
        thread->Delete();
        return kControllerCantRunThread;
    }

    if( load_thread_ )
        load_thread_->Delete();

    handler_ = opened_handler;
    load_thread_ = thread;
    
    return kControllerSuccess;
}

void Controller::Zoom( const wxPoint &area, const wxPoint &pos, const ControllerZoom &flag )
{

}

void Controller::SetBitmap( std::shared_ptr<Bitmap> bitmap )
{
    bitmap_ = bitmap;
}

void Controller::SetHandler( std::shared_ptr<AbstractHandler> handler )
{
    handler_ = handler;
}

wxThread *Controller::GetThread( int id )
{
    switch ( id )
    {
        case kLoadThreadID:
            return load_thread_;
        case kZoomThreadID:
            return zoom_thread_;
    }
    return NULL;
}

void Controller::DoSetNull( int id )
{
    switch ( id )
    {
        case kLoadThreadID:
            load_thread_ = NULL;
            break;
        case kZoomThreadID:
            zoom_thread_ = NULL;
            break;
    }
}

void Controller::Clear()
{
    Delete( kLoadThreadID, g_sLock );
    Delete( kZoomThreadID, g_sLock );
    bitmap_.reset();
    handler_.reset();
}

} // namespace reader

} // namespace fmr
