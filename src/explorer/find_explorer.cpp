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

#include "explorer/find_explorer.h"
#include "handler/abstract_handler.h"
#include "handler/handler_factory.h"
#include <wx/image.h>
#include <wx/filename.h>

namespace fmr
{

namespace explorer
{

wxDEFINE_EVENT( EVT_STREAM_FOUND, StreamEvent );

void FindThread::SetParameter( std::vector<StreamBitmap> &list_stream )
{
    list_stream_ = list_stream;
}

wxThread::ExitCode FindThread::Entry()
{
    for ( auto &it : list_stream_  )
    {
        if (!TestDestroy() )
            Find( it );

        Update();
        if ( TestDestroy() )
            break;
    }
    Completed();
    return (wxThread::ExitCode)0;
}

bool FindThread::Find( StreamBitmap &item )
{
    wxImage img;

    if ( ! item.stream->IsOk() )
        item.stream->Open( item.stream->GetName() );

    if ( wxImage::CanRead( *item.stream->GetStream() ) )
    {
        StreamEvent *event = new StreamEvent( EVT_STREAM_FOUND, m_id );
        size_t idx = 0, index = 0;

        for ( const auto &it : list_stream_ )
        {
            if ( it.bitmap == item.bitmap )
            {
                idx = index;
                break;
            }
            index++;
        }

        event->SetIndex( idx );
        event->SetStream( std::shared_ptr<SStream>(item.stream) );

        wxQueueEvent(
            GetParent(),
            event
        );

        return true;
    }
    else
    {
        HandlerType type;
        HandlerFactory::Find( item.stream->GetName(), type );

        std::shared_ptr<AbstractHandler> handler(
            HandlerFactory::NewHandler( type )
        );

        if ( !handler )
            return false;

        handler->Open( item.stream->GetName() );

        if ( item.stream->GetName() == handler->GetName() && !TestDestroy() )
        {
            bool isGetStream = type != kHandlerDefault;
            handler->Traverse( isGetStream );
            for ( auto &it : handler->GetChild() )
            {
                if ( TestDestroy() )
                    break;

                StreamBitmap temp;
                temp.stream = std::shared_ptr<SStream>(
                    new SStream( it )
                );

                temp.bitmap = item.bitmap;
                if ( !TestDestroy() && Find( temp ) )
                {
                    return true;
                }
            }
        }
    }
    return false;
}

}; // namespace explorer

}; // namespace fmr