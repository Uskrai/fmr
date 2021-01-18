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

#ifndef FMR_EXPLORER_COMMON
#define FMR_EXPLORER_COMMON

#include <fmr/bitmap/bmp.h>
#include <fmr/handler/struct_stream.h>

#include <memory>

namespace fmr
{

namespace explorer
{

struct StreamBitmap
{
    std::shared_ptr<SBitmap> bitmap = NULL;
    std::shared_ptr<SStream> stream = NULL;
};

struct StringDraw
{
    std::string filename;
    wxRect rect;
};

class StreamBitmapEvent
    : public wxCommandEvent
{
    public:
        StreamBitmapEvent( wxEventType type = wxEVT_NULL, int id = -1 )
            : wxCommandEvent( type, id ) {}

        StreamBitmapEvent( const StreamBitmapEvent &other )
            : wxCommandEvent( other.GetEventType(), other.GetId() )
        {
            stream_bitmap_ = other.stream_bitmap_;
        }

        using wxCommandEvent::Clone;
        wxEvent *Clone() { return new StreamBitmapEvent( *this ); }

        StreamBitmap &GetStreamBitmap() { return stream_bitmap_; }

        void SetStreamBitmap( const StreamBitmap &item )
            { stream_bitmap_ = item;}
    protected:
        StreamBitmap stream_bitmap_;
};

typedef void (wxEvtHandler::*StreamBitmapEventFunction)(StreamBitmapEvent&);

#define StreamBitmapEventHandler( func ) wxEVENT_HANDLER_CAST( StreamBitmapEventFunction, func )

#define EVT_STREAM_BITMAP( id, type, func )    \
    wx__DECLARE_EVT1( type, id, StreamBitmapEventHandler( func ) )

}; // namespace explorer

}; // namespace fmr

#endif
