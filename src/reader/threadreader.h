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
#include <wx/event.h>
#include <wx/string.h>

#include "handler/handler.h"


class ScrolledWindow;
class Bitmap;

wxDECLARE_EVENT( EVT_COMMAND_LOADTHREAD_UPDATE, wxThreadEvent );
wxDECLARE_EVENT( EVT_COMMAND_LOADTHREAD_COMPLETED, wxThreadEvent );
namespace Reader
{

inline wxCriticalSection LoadThreadLock;
class LoadThread
    : public wxThread
{
    public:
        LoadThread( ScrolledWindow *parent, const wxThreadKind &type = wxTHREAD_DETACHED );
        void SetParameter( Bitmap *bitmap, Handler *handler, size_t start );
        void SetLimit( size_t prev, size_t next );
        ~LoadThread();

        wxThreadError Run();

        wxCriticalSection &GetLock() { return LoadThreadLock; }
    protected:
        size_t m_start;
        Bitmap *m_bitmap = NULL;
        Handler *m_fHandler;
        virtual ExitCode Entry();
        wxWindow *m_parent;
};

};

#endif