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

#include "base/vector.h"
#include "base/range.h"

class wxScrolledWindow;
class Bitmap;
class wxString;

namespace Reader
{

class Thread
    : public wxThreadHelper
{
    public:
        Thread( wxScrolledWindow* parent, Bitmap* bitmap );
        ~Thread();

        void Open( const wxString& path );
        bool IsOpened() { return m_isOpened; }
        Handler* GetHandler() { return m_handler; }

        void Clear();
        void SetHandler( Handler* handler );
        void SetLimit( int prev = NO_LIMIT , int next = NO_LIMIT ) { m_limitPrev = prev, m_limitNext = next; }

        wxCriticalSection& GetLock() { return this->gCS; }
        wxScrolledWindow* GetParent() { return m_parent; }
        
    protected:
        wxCriticalSection gCS;
        Bitmap* m_bitmap;
        wxScrolledWindow* m_parent;
        Handler* m_handler = NULL;
        wxString m_path;

        bool m_isOpened = false;

        wxThread::ExitCode Entry();
        void BitmapThread( bool& destroy, size_t current );

        int m_limitNext = NO_LIMIT, m_limitPrev = NO_LIMIT;
        bool m_threadbmp = false;

        bool TestDestroy();

        bool IsExist( int idx );

        bool LoadImage( size_t idx, bool isScroll = false );
        void LoadBitmap( int current, int prev, int next, bool& isDestroyed );
    
    private:
        size_t m_curr = 0;
        template<typename T>
        T ConfRead( const wxString& name, T def );
};

};

#endif