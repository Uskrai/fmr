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

#include "reader/controller_reader.h"
#include "handler/handler_factory.h"

namespace fmr
{

namespace reader
{

Controller::Controller( wxWindow *parent )
{
    parent_ = parent;
}

ControllerExit Controller::Open( const wxString &path )
{
    if ( path.IsEmpty() )
        return kControllerPathEmpty;
    
    if ( ! bitmap_ )
        return kControllerBitmapEmpty;
    
    auto opened_handler = std::shared_ptr<AbstractHandler>(
        HandlerFactory::NewHandler( path )
    );

    if ( ! opened_handler  )
        return kControllerHandlerNotFound;

    opened_handler->Traverse( true );

    if ( opened_handler->Size() == 0 )
        return kControllerFolderEmpty;

    size_t idx = opened_handler->Index( path );
    LoadThread::LoadImage( opened_handler, bitmap_, idx );

    if ( load_thread_ )
        load_thread_->Delete();
    
    load_thread_ = new LoadThread( this, wxTHREAD_DETACHED, kLoadThreadID );
    load_thread_->SetParameter( opened_handler, bitmap_, idx );
    
    if ( load_thread_->Run() != wxTHREAD_NO_ERROR )
        return kControllerCantRunThread;
    
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

} // namespace reader

} // namespace fmr