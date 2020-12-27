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
#include "handler/handler_factory.h"

namespace fmr
{

namespace explorer
{

void LoadThread::SetSize( const wxSize &size )
{
    image_size_ = size;
}

void LoadThread::SetImageQuality( wxImageResizeQuality quality )
{
    image_quality_ = quality;
}

void LoadThread::Push( StreamBitmap &stream_bitmap )
{
    load_queue_.push( stream_bitmap );
}

void LoadThread::Clear()
{
    load_queue_ = std::queue<StreamBitmap>();
}

void LoadThread::DeleteOnEmptyQueue( bool condition )
    { is_delete_on_empty_ = condition; }

void LoadThread::Load( StreamBitmap &stream )
{
    wxImage image;
    if ( ! wxImage::CanRead( *stream.stream->GetStream() ) )
        return;

    if ( !TestDestroy() )
        image_util::Load( image, *stream.stream->GetStream() );

    if ( image.IsOk() && !TestDestroy() )
        image_util::Rescale( image, image_size_, image_quality_ );

    if ( image.IsOk() && !TestDestroy() )
        stream.bitmap->SetBitmap( image );

}

wxThread::ExitCode LoadThread::Entry()
{
    #define TEST_BREAK()       \
        if ( TestDestroy() )    \
            break

    while ( !TestDestroy() &&  !( is_delete_on_empty_ && load_queue_.empty() ) )
    {
        if ( load_queue_.size() > 0 && !TestDestroy() )
        {
            StreamBitmap &stream = load_queue_.front();
            std::shared_ptr<wxInputStream> input_stream = stream.stream->GetStream();

            TEST_BREAK();

            if ( !TestDestroy() && wxImage::CanRead( *input_stream ) )
            {
                Load( stream );
            }
            else
            {
                TEST_BREAK();
                std::shared_ptr<AbstractHandler> handler(
                    HandlerFactory::NewHandler( stream.stream->GetHandlerPath() )
                );

                handler->Traverse( true );

                TEST_BREAK();

                size_t index = handler->Index( stream.stream->GetName() );

                if ( handler->IsExist( index ) && !TestDestroy() )
                    stream.stream = std::shared_ptr<SStream>(
                        new SStream( handler->Item( index ) )
                    );

                TEST_BREAK();

                Load( stream );
            }

            TEST_BREAK();

            Update();
            load_queue_.pop();
        }

        TEST_BREAK();
    }

    Completed();

    return (wxThread::ExitCode)0;
}

}

}; // namespace fmr