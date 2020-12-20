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

#include "thread/thread.h"

#include "window/scrolledwindow.h"

namespace fmr
{

wxDEFINE_EVENT( EVT_COMMAND_THREAD_UPDATE, wxThreadEvent );
wxDEFINE_EVENT( EVT_COMMAND_THREAD_COMPLETED, wxThreadEvent );

bool ThreadController::DeleteThread( wxThread * const &thread, wxCriticalSection &lock )
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

        wxThread::This()->Sleep(1);
    }
    return true;
}

BaseThread::BaseThread( ThreadController *parent, wxThreadKind type, int id )
{
    m_parent = parent;
    SetId(id);
}

void BaseThread::SetId( int id )
{
    wxCriticalSectionLocker locker(g_sLock);
    m_id = id;
}

void BaseThread::Update()
{
    wxQueueEvent( 
        m_parent,
        new wxThreadEvent(
            EVT_COMMAND_THREAD_UPDATE,
            m_id
        )
    );
}

void BaseThread::Completed()
{
    wxQueueEvent(
        m_parent,
        new wxThreadEvent(
            EVT_COMMAND_THREAD_COMPLETED
        )
    );
}

BaseThread::~BaseThread()
{
    wxCriticalSectionLocker locker(g_sLock);
    m_parent->DoSetNull(m_id);
}

}; // namespace fmr