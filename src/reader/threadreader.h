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

#ifndef FMR_READER_THREAD
#define FMR_READER_THREAD

#include <wx/thread.h>
#include <wx/string.h>

#include "handler/handler.h"

class wxScrolledWindow;
class Image; 
class BitmapVertical;
class wxString;

namespace Reader
{

class Thread
    : public wxThreadHelper
{
    public:
        Thread( wxScrolledWindow* parent, Image* image, BitmapVertical* bitmap );
        ~Thread();

        void Open( const wxString& path );
        void Clear();
        void SetHandler( Handler* handler );

        wxCriticalSection& GetLock() { return this->gCS; }
    protected:
        wxCriticalSection gCS;
        Image* m_image;
        BitmapVertical* m_bitmap;
        wxScrolledWindow* m_parent;
        Handler* m_handler = NULL;
        wxString m_path;
        Handler* GetHandler(){ return m_handler; }

        wxThread::ExitCode Entry();
        void BitmapThread( bool& destroy );

        int m_next, m_prev;
        bool m_threadbmp = false;

        bool TestDestroy();

        bool IsExist( int idx );

        bool LoadImage( size_t idx, int pos );
};

};

#endif