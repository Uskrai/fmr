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

wxThread::ExitCode LoadThread::Entry()
{
    while ( !TestDestroy() )
    {
        if ( load_queue_.size() > 0 )
        {
            StreamBitmap &stream = load_queue_.front();
            wxImage image;

            if ( wxImage::CanRead( *stream.stream->GetStream() ) )
            {
                if ( !TestDestroy() )
                    image_util::Load( image, *stream.stream->GetStream() );

                if ( image.IsOk() && !TestDestroy() )
                    image_util::Rescale( image, image_size_, image_quality_ );

                if ( image.IsOk() && !TestDestroy() )
                    stream.bitmap->SetBitmap( image );

                Update();
            }

            load_queue_.pop();
        }
    }

    return (wxThread::ExitCode)0;
}

}

}; // namespace fmr