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
#include <functional>

class ScrolledWindow;
class Handler;
class Bitmap;
class wxImage;
struct SBitmap;


class ZoomThread :
    public wxThread
{
    public :
        ZoomThread( ScrolledWindow *parent, const wxThreadKind type = wxTHREAD_DETACHED, int id = wxID_ANY );
        ~ZoomThread();
        static void Zoom( SBitmap *bmp, wxImage &img, float scale, std::function<bool()> NotDestroyed = []()->bool { return true; } );

        void SetParameter( Handler *handler, Bitmap *bitmap, float scale );

    protected :
        virtual ExitCode Entry();
        ScrolledWindow *m_parent;
        Handler *m_handler;
        Bitmap *m_bitmap;
        float m_scale;
        int m_id;
};

#endif