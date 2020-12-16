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

#ifndef FMR_READER_ZOOMREADER
#define FMR_READER_ZOOMREADER

#include "thread/thread.h"

#include "bitmap/bmp.h"
#include "bitmap/bitmap.h"
#include "window/scrolledwindow.h"
#include "handler/struct_stream.h"
#include "handler/abstract_handler.h"

#include <functional>
#include <memory>

class wxImage;


namespace fmr
{

namespace reader
{

class ZoomThread :
    public BaseThread
{
    public :
        ZoomThread( ScrolledWindow *parent, const wxThreadKind type = wxTHREAD_DETACHED, int id = wxID_ANY )
            : BaseThread(parent,type,id){} ;
        ~ZoomThread();
        static void Zoom( SBitmap *bmp, wxImage &img, float scale, std::function<bool()> NotDestroyed = []()->bool { return true; } );
        static void Zoom( SBitmap *bmp, SStream &stream, float scale, std::function<bool()> NotDestroyed = []()->bool {return true;} );

        void SetParameter( std::shared_ptr<AbstractHandler> handler, std::shared_ptr<Bitmap> bitmap, float scale );

    protected :
        virtual ExitCode Entry();
        std::shared_ptr<AbstractHandler> m_handler;
        std::shared_ptr<Bitmap> m_bitmap;
        float m_scale;
};

}; // namespace reader

}; // namespace fmr

#endif