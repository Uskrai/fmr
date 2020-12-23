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

#include "explorer/load_explorer.h"
#include "bitmap/image_util.h"
#include "handler/abstract_handler.h"
#include "handler/handler_factory.h"
#include <wx/image.h>
#include <wx/filename.h>

namespace fmr
{

namespace explorer
{

void LoadThread::SetParameter( std::vector<StreamBitmap> &list_stream )
{
    list_stream_ = list_stream;
}

wxThread::ExitCode LoadThread::Entry()
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

bool LoadThread::Find( StreamBitmap &item )
{
    wxImage img;

    if ( ! item.stream->IsOk() )
        item.stream->Open( item.stream->GetName() );

    if ( !TestDestroy() && image_util::Load( img, *item.stream ) )
    {
        if ( !TestDestroy() )
            image_util::Rescale( img, wxSize(300,300) );

        if ( !TestDestroy() )
            item.bitmap->SetBitmap( img );
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
                printf("%d\n", it.IsOk() );
                if ( TestDestroy() )
                    break;

                StreamBitmap temp;
                temp.stream = &it;
                temp.bitmap = item.bitmap;
                if ( !TestDestroy() && Find( temp ) )
                    return true;
            }
        }
    }
    return false;
}

}; // namespace explorer

}; // namespace fmr