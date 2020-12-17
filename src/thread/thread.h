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

#ifndef FMR_THREAD_THREAD
#define FMR_THREAD_THREAD

#include <wx/event.h>
#include <wx/thread.h>

wxDECLARE_EVENT( EVT_COMMAND_THREAD_UPDATE, wxThreadEvent );
wxDECLARE_EVENT( EVT_COMMAND_THREAD_COMPLETED, wxThreadEvent );

class ScrolledWindow;

inline wxCriticalSection g_sLock;

class BaseThread : public wxThread
{
    public:
        BaseThread( ScrolledWindow *parent, const wxThreadKind type = wxTHREAD_DETACHED, int id = -1 );
        ~BaseThread();

        void SetId( int id );
    protected:
        void Update();
        void Completed();
        ScrolledWindow *m_parent;
        int m_id;
};

namespace Thread
{
    template <typename T>
    std::enable_if_t
    <std::is_base_of<BaseThread, T>::value, bool> Delete( T *&thread, wxCriticalSection &lock )
    {
        {
            wxCriticalSectionLocker locker(lock);
            if ( thread )
                if ( thread->Delete() != wxTHREAD_NO_ERROR )
                    return false;
        }

        while (1)
        {
            {
                wxCriticalSectionLocker locker(lock);
                if ( !thread ) break;
            }

            wxThread::This()->Sleep(2);
        }
        return true;
    }
}

#endif