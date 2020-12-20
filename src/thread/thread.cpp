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

void ThreadUpdate( wxEvtHandler *dest, int thread_id )
{
    wxQueueEvent( 
        dest,
        new wxThreadEvent(
            EVT_COMMAND_THREAD_UPDATE,
            thread_id
        )
    );
}

void ThreadCompleted( wxEvtHandler *dest, int thread_id )
{
    wxQueueEvent(
        dest,
        new wxThreadEvent(
            EVT_COMMAND_THREAD_COMPLETED,
            thread_id
        )
    );
}

wxBEGIN_EVENT_TABLE( ThreadController, wxEvtHandler )
    EVT_COMMAND( wxID_ANY, EVT_COMMAND_THREAD_UPDATE, ThreadController::OnUpdate )
    EVT_COMMAND( wxID_ANY, EVT_COMMAND_THREAD_COMPLETED, ThreadController::OnCompleted )
wxEND_EVENT_TABLE()

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

void ThreadController::Update( int id )
{
    ThreadUpdate(
        GetParent(),
        id
    );
}

void ThreadController::Completed( int id )
{
    ThreadCompleted(
        GetParent(),
        id
    );
}

void ThreadController::OnUpdate( wxCommandEvent &event )
{
    Update( event.GetId() );
}

void ThreadController::OnCompleted( wxCommandEvent &event )
{
    Completed( event.GetId() );
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
    ThreadUpdate( m_parent, m_id );
}

void BaseThread::Completed()
{
    ThreadCompleted( m_parent, m_id );
}


BaseThread::~BaseThread()
{
    wxCriticalSectionLocker locker(g_sLock);
    m_parent->DoSetNull(m_id);
}

}; // namespace fmr