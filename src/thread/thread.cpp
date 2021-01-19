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

#include <fmr/thread/thread.h>

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

ThreadController::ThreadController()
{
    BindEvent();
}

void ThreadController::BindEvent()
{
    Bind( EVT_COMMAND_THREAD_UPDATE, &ThreadController::OnUpdate, this );
    Bind( EVT_COMMAND_THREAD_COMPLETED, &ThreadController::OnCompleted, this );
}

bool ThreadController::Delete( int thread_id, wxCriticalSection &lock )
{
    wxCriticalSectionLocker locker(lock);
    if ( GetThread( thread_id ) )
        return GetThread( thread_id )->Delete() == wxTHREAD_NO_ERROR;
    return false;
}

bool ThreadController::Wait( int thread_id, wxCriticalSection &lock )
{
    while ( true )
    {
        {
            wxCriticalSectionLocker locker(lock);
            if ( ! GetThread( thread_id ) ) break;
        }
        wxThread::This()->Sleep( 1 );
    }
    return true;
}

bool ThreadController::DeleteThread( int thread_id, wxCriticalSection &lock )
{
    if ( Delete( thread_id, lock ) )
    {
        return Wait( thread_id, lock );

    }
    return false;

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

void ThreadController::OnUpdate( wxThreadEvent &event )
{
    Update( event.GetId() );
}

void ThreadController::OnCompleted( wxThreadEvent &event )
{
    Completed( event.GetId() );
}

BaseThread::BaseThread( ThreadController *parent, wxThreadKind type, int id )
    : wxThread( type )
{
    parent_ = parent;
    SetId(id);
}

void BaseThread::SetId( int id )
{
    wxCriticalSectionLocker locker(g_sLock);
    m_id = id;
}

void BaseThread::QueueEventParent( wxEvent *event )
{
    if ( disable_event_on_destroy_ )
    {
        if ( wxThread::This() && GetId() == wxThread::This()->GetId() )
        {
            if ( TestDestroy() )
            {
                delete event;
                return;
            }
        }
    }

    if ( event )
        event->SetEventObject( this );

    wxQueueEvent(
        GetParent(),
        event
    );
}

void BaseThread::Update()
{
    wxThreadEvent *event = new wxThreadEvent(
        EVT_COMMAND_THREAD_UPDATE,
        m_id
    );

    QueueEventParent( event );
}

void BaseThread::Completed()
{
    wxThreadEvent *event = new wxThreadEvent(
        EVT_COMMAND_THREAD_COMPLETED,
        m_id
    );

    QueueEventParent( event );
}

void BaseThread::DisableEventOnDestroy( bool disable )
    { disable_event_on_destroy_ = disable;}


BaseThread::~BaseThread()
{
    wxCriticalSectionLocker locker(g_sLock);
    if ( GetParent() )
        GetParent()->DoSetNull(m_id);
}

}; // namespace fmr
