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

#include "reader/threadreader.h"

#include <wx/scrolwin.h>
#include "image/image.h"
#include "bitmap/bitmapvertical.h"

#include <thread>
#include <future>
namespace Reader
{

Thread::Thread( wxScrolledWindow* parent, BitmapVertical* bitmap )
{
    m_bitmap = bitmap;
    m_parent = parent;
}

Thread::~Thread() 
{
    Clear();
}

void Thread::SetHandler( Handler* handler )
{
    if ( m_handler ) delete m_handler;
    m_handler = handler;
}

void Thread::Open( const wxString& path )
{
    m_path = path;
    m_threadbmp = true; // activate bitrmap thread;
    
    // return if thread already running
    if ( GetThread() && GetThread()->IsRunning() ) return; 
    if ( CreateThread( wxTHREAD_JOINABLE ) != wxTHREAD_NO_ERROR )
    {
        return;
    }
    GetThread()->Run();
}

void Thread::Clear()
{
    m_threadbmp = false;
    
    if ( GetThread() && GetThread()->IsRunning() )
        GetThread()->Delete();

    if ( m_handler )
    {
        delete m_handler;
        m_handler = NULL;
    }
}

bool Thread::TestDestroy()
{
    return this->GetThread()->TestDestroy();
}

// template<typename T>
// T Thread::ConfRead( const wxString& name, T def )
// { 
//     return m_config->Read( wxString("Reader/") + name, def ); 
// }

template<typename T>
bool IsReady( std::future<T>& t )  // to check if the thread completed or not
{
    return t.wait_for( std::chrono::microseconds(0) ) == std::future_status::ready;
}

wxThread::ExitCode Thread::Entry()
{
    bool destroy(false);


    std::future<void> bmp;
    while ( true ) // for checking whether the thread finishes or the thread
    {
        if ( m_threadbmp )
        {
            bmp = std::async( std::launch::async, &Thread::BitmapThread, this, std::ref(destroy) );
            m_threadbmp = false;
        }

        if ( TestDestroy() )
        {
            destroy = true;
            break;
        }

        if ( IsReady( bmp ) )
        {
            break;
        }
    }
    return (wxThread::ExitCode)0;
}

bool Thread::IsExist( int idx )
{
    return GetHandler()->IsExist( idx );
}

bool Thread::LoadImage( size_t idx, bool isScroll )
{
    if ( IsExist(idx) )
    {
        wxInputStream* stream = GetHandler()->Item(idx);
        if ( wxImage::CanRead( *stream ) )
        {
            wxImage img = wxImage(*stream);
            m_bitmap->Add( img, idx, isScroll );
            m_bitmap->Refresh();
            m_bitmap->RefreshPosition();
            return true;
        }
    }
    
    return false;
}

void Thread::BitmapThread( bool& isDestroyed  )
{
    GetHandler()->Clear();
    GetHandler()->Open( m_path );
    GetHandler()->Traverse();

    int curr = GetHandler()->Index( m_path ),
        prev = curr - 1, next = curr + 1;

    m_bitmap->GetAll().assign( GetHandler()->Size(), SBitmap() );

    if ( IsExist(curr) )
    {
        LoadImage( curr );
    }

    while ( !isDestroyed && ( IsExist(prev) || IsExist(next) ) )
    {
        if ( !isDestroyed && IsExist(next) )
        {
            LoadImage( next++ );
        }
        if ( !isDestroyed && IsExist(prev) )
        {
            LoadImage( prev--, true );
        }

        if ( ( !IsExist(next) && !IsExist(prev) ) ) break;
    }
}

} // namespace reader