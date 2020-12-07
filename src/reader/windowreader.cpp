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

#include "reader/windowreader.h"

#include "thread/thread.h"
#include "reader/loadreader.h"
#include "base/config.h"

#include "bitmap/bitmap.h"
#include "handler/handlerfactory.h"


#include <wx/log.h>
#include <wx/scrolbar.h>
#include <wx/sizer.h>
#include <wx/dcclient.h>

// TODO : Make separate thread controller

namespace Reader
{

#define Free( var )         \
    if ( var )              \
    {                       \
        delete var;         \
        var = NULL;         \
    }

wxBEGIN_EVENT_TABLE( Window, ScrolledWindow )
    EVT_MOTION( Window::OnMouseMotion )
    EVT_COMMAND( wxID_ANY, EVT_COMMAND_THREAD_UPDATE, Window::OnThreadUpdate )
    EVT_COMMAND( wxID_ANY, EVT_COMMAND_THREAD_COMPLETED, Window::OnThreadComplete)
wxEND_EVENT_TABLE()

Window::Window( wxWindow* parent, wxWindowID id, const wxPoint & pos, 
                const wxSize &size, long style, const wxString &name ) :
    ScrolledWindow( parent, id, wxDefaultPosition, size, style, name )
{
    m_config = Config::Get();
};

Window::~Window()
{
}

bool Window::Destroy()
{
    Clear();
    return wxWindow::Destroy();
}

void Window::Clear()
{
    if ( m_loadThread ) 
        m_loadThread->Delete();
    Free(m_loadThread)
    Free(m_fileHandler)
    Free(m_bitmap)
}

void Window::OnDraw( wxDC &dc )
{   
    dc.SetClippingRegion( GetViewStart(), GetClientSize() );
    if ( m_bitmap )
    {
        wxCriticalSectionLocker locker( g_sLock );
        wxVector<SBitmap*> vec = m_bitmap->Get();
        for ( const auto& it : vec )
        {
            if ( it->IsOk() )
                dc.DrawBitmap( it->GetBitmap() , it->GetPosition() );
        }
    }
}


void Window::OnThreadComplete( wxCommandEvent &event )
{
    Free(m_loadThread)
}

bool Window::Open( const wxString& path )
{
    // open if path is not empty
    if ( path != wxEmptyString )
    {
        Handler *tempHandler = m_fileHandler;
        Bitmap *tempBitmap = m_bitmap;
        LoadThread *tempThread = m_loadThread;
        SetVirtualSize( GetClientSize() + wxSize(1,0) );

        Handler *handler = NewHandler( path );
        // if handler found prepare for runing thread
        if ( handler )
        {
            if ( handler->Size() > 0 )
            {
                m_config->Write("RecentlyOpened", path );
                ReloadConfig();
                size_t limitPrev = ConfRead("ImageMemoryLimitPrev", NO_LIMIT );
                size_t limitNext = ConfRead("ImageMemoryLimitNext", NO_LIMIT );
                size_t limitImage = ConfRead("ImageShowLimit", 1 );

                Bitmap *bitmap = NewBitmap( limitImage, handler->Size() );
                size_t idx = handler->Index( path );

                LoadThread *thread;
                thread = new LoadThread( this, wxTHREAD_JOINABLE );
                thread->SetParameter( bitmap, handler, idx );
                thread->SetLimit( limitPrev, limitNext );

                if ( thread->Run() != wxTHREAD_NO_ERROR )
                {
                    wxLogError("Can't Create Thread");
                    Free(handler)
                    Free(bitmap)
                    Free(thread)

                    m_fileHandler = tempHandler;
                    m_loadThread = tempThread;
                    m_bitmap = tempBitmap;
                    return false;
                }

                Clear();

                m_fileHandler = handler;
                m_bitmap = bitmap;
                m_loadThread = thread;
                return true;
            }
            else
                wxLogStatus( path + " doesn't have any image");
        } // end of if m_filehandler not null
    } // end of if path not empty
    return false;
}

Handler *Window::NewHandler( const wxString &path )
{
    Handler *handler = HandlerFactory::NewHandler(path);

    if ( handler )
    {
        handler->Open(path);
        handler->Traverse( true );
        if ( handler->GetParent() )
            handler->GetParent()->Traverse();
    }
    
    return handler;
}

Bitmap *Window::NewBitmap( size_t size, size_t limit )
{
    Bitmap *bitmap = NULL;
    bitmap = new Bitmap( this );

    bitmap->SetLimit( size );
    bitmap->Resize( limit );
    bitmap->GetAll().assign( limit,  SBitmap() );

    int scale = ConfRead("ImageScaleFromOriginal", 100 );
    long pos = ConfRead("ImagePosition", long(BITMAP_VERTICAL | BITMAP_CENTERED) );
    long sizeflag = ConfRead("ImageSize", long(BITMAP_ORIGINAL) );
    bitmap->SetFlags(pos,sizeflag,scale);

    return bitmap;
}

void Window::ReloadConfig()
{
    m_config->Flush();
}

bool Window::ChangeFolder( int step )
{
    wxString path = m_fileHandler->GetFromCurrent( step );

    if ( path == wxEmptyString ) return false;

    if ( Open( path ) ) return true;

    return ChangeFolder( step + step );

}

void Window::Next() { Open( GetHandler()->GetNext()); }
void Window::Prev() { Open( GetHandler()->GetPrev()); }

void Window::Find( const wxString& path )
{
}

// void Window::OnDraw( wxDC& dc )
template<typename T>
T Window::ConfRead( wxString name, T def )
{ 
    return m_config->Read( wxString("Reader/") + name, def ); 
}

void Window::OnEdge( wxDirection direction )
{
    if ( m_bitmap )
    {
        int step = 0;
        if ( direction == wxUP || direction == wxLEFT )
            step = -1;
        
        if ( direction == wxDOWN || direction == wxRIGHT )
            step = 1;

        BITMAP_PAGES status = m_bitmap->ChangePage( step );
        int x = ( m_isFromRight ) ? GetVirtualSize().GetWidth() : 0;
        if ( status == BITMAP_ENDOFPAGE )
            if ( ChangeFolder( step ) )
                Scroll(  GetVirtualSize().GetWidth() , 0 );
        
        if ( status == BITMAP_CHANGEPAGE )
        {
            if ( direction == wxUP )
                Scroll( 0 , GetVirtualSize().GetHeight() );
            
            else if ( direction == wxDOWN )
                Scroll( x, 0 );
            
            else if ( direction == wxLEFT )
                Scroll( x, 0 );
            
            else if ( direction == wxRIGHT )
                Scroll( x, 0 );
        }
    }
}

}
