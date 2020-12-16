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

#include "reader/window_reader.h"

#include "base/config.h"
#include "bitmap/bitmap.h"
#include "handler/handler_factory.h"

#include <wx/log.h>
#include <wx/scrolbar.h>
#include <wx/sizer.h>
#include <wx/dcclient.h>
#include <wx/wxcrtvararg.h>

// TODO : Make separate thread controller

namespace fmr
{

namespace reader
{

enum ThreadID : int
{
    LoadThreadID = wxID_HIGHEST + 30,
    ZoomThreadID
};

#define Free( var )         \
    if ( var )              \
    {                       \
        delete var;         \
        var = NULL;         \
    }

wxBEGIN_EVENT_TABLE( Window, ScrolledWindow )
    EVT_SIZE( Window::OnSize )
    EVT_MOTION( Window::OnMouseMotion )
    EVT_COMMAND( LoadThreadID, EVT_COMMAND_THREAD_UPDATE, Window::OnThreadUpdate )
    EVT_COMMAND( ZoomThreadID, EVT_COMMAND_THREAD_UPDATE, Window::OnThreadUpdate )
    EVT_COMMAND( ZoomThreadID, EVT_COMMAND_THREAD_COMPLETED, Window::OnZoomThreadCompleted )
wxEND_EVENT_TABLE()

Window::Window( wxWindow* parent, wxWindowID id, const wxPoint & pos, 
                const wxSize &size, long style, const wxString &name ) :
    ScrolledWindow( parent, id, wxDefaultPosition, size, style, name )
{
    m_config = Config::Get();
    CalcScrollStep( 
        static_cast<ScrollingType>(ConfRead("ScrollType", int(SCROLL_BY_WINDOW) ) ) 
    );
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
    Thread::Delete( m_loadThread, g_sLock );
    Thread::Delete( m_zoomThread, g_sLock );
}

void Window::OnDraw( wxDC &dc )
{   
    dc.SetClippingRegion( GetViewStart(), GetClientSize() );
    // wxCriticalSectionLocker locker( g_sLock );
    if ( m_bitmap )
    {
        wxVector<SBitmap*> vec = m_bitmap->Get( );
        for ( const auto& it : vec )
        {
            if ( it->IsOk() && it->IsShown( GetViewStart(), GetClientSize() ) )
                dc.DrawBitmap( it->GetBitmap() , it->GetPosition() );
        }
    }
}

void Window::OnSize( wxSizeEvent &event )
{
    wxSize sz = event.GetSize();
    if ( m_bitmap )
        AdjustBitmap();
    else 
        SetVirtualSize( sz );
    
    Refresh();
    event.Skip();
}

void Window::AdjustBitmap()
{
    if ( m_bitmap )
    {
        m_bitmap->Refresh();
        wxSize sz = m_bitmap->GetSize( GetClientSize() );
        m_bitmap->RefreshPosition( sz );
        SetVirtualSize( sz );
    }
}

void Window::CalcScrollStep( ScrollingType type )
{
    if ( type == SCROLL_BY_WINDOW )
    {
        int percent = ConfRead("ScrollPerKeyByWindow",30);
        m_stepPerKey = GetClientSize().y * percent / 100 ;
    }
    else if ( type == SCROLL_BY_IMAGE )
    {
        // TODO : call this when changing virtual size
        int percent = ConfRead("ScrollPerKeyByImage",20);
        m_stepPerKey = GetVirtualSize().y * percent / 100;
    }
    else if ( type == SCROLL_BY_PIXEL )
    {
        m_stepPerKey = ConfRead("ScrollPerKeyByPixel",300);
    }

}

void Window::DoSetNull( int id)
{
    switch ( id )
    {
        case LoadThreadID:
            m_loadThread = NULL;
            break;
        case ZoomThreadID:
            m_zoomThread = NULL;
            break;
    }
}

void Window::OnThreadUpdate( wxCommandEvent &event )
{
    AdjustBitmap();
    Refresh();
}

bool Window::Open( const wxString& path )
{
    // open if path is not empty
    if ( path != wxEmptyString )
    {
        SetVirtualSize( GetClientSize() + wxSize(1,0) );

        std::shared_ptr<AbstractHandler> handler = NewHandler( path );
        // if handler found prepare for runing thread
        if ( handler )
        {
            m_isOpened = false;
            if ( handler->Size() > 0 )
            {
                size_t limitPrev = ConfRead("ImageMemoryLimitPrev", NO_LIMIT );
                size_t limitNext = ConfRead("ImageMemoryLimitNext", NO_LIMIT );
                size_t limitImage = ConfRead("ImageShowLimit", 1 );

                std::shared_ptr<Bitmap> bitmap = NewBitmap( limitImage, handler->Size() );
                wxSize sz;
                size_t idx = handler->Index( path );
                if ( handler->IsExist( idx ) )
                {
                    LoadThread::LoadImage( bitmap, handler->Item(idx), idx );
                    printf("owo\n");
                    bitmap->Refresh();
                    sz = bitmap->GetSize( GetClientSize() );
                    bitmap->RefreshPosition(sz);
                    Refresh();
                    Scroll(0,0);
                }
                std::shared_ptr<AbstractHandler> handler = NewHandler( path );

                LoadThread *thread;
                thread = new LoadThread( this, wxTHREAD_DETACHED, LoadThreadID );
                thread->SetParameter( handler, bitmap, idx );
                thread->SetLimit( limitPrev, limitNext );

                m_isOpened = true;
                if ( thread->Run() != wxTHREAD_NO_ERROR )
                {
                    wxLogError("Can't Create Thread");
                    return false;
                }
                SetVirtualSize( sz );
                AdjustScrollBar();

                m_config->Write("RecentlyOpened", path );
                ReloadConfig();

                if ( m_loadThread )
                {
                    m_loadThread->SetId(-1);
                    m_loadThread->Delete();
                }

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

std::shared_ptr<AbstractHandler> Window::NewHandler( const wxString &path )
{
    std::shared_ptr<AbstractHandler> handler;
    handler = std::shared_ptr<AbstractHandler>(HandlerFactory::NewHandler(path));

    if ( handler )
    {
        handler->Open(path);
        handler->Traverse( true );
        if ( handler->GetParent() )
            handler->GetParent()->Traverse();
    }
    
    return handler;
}

std::shared_ptr<Bitmap> Window::NewBitmap( size_t size, size_t limit )
{
    std::shared_ptr<Bitmap> bitmap;
    bitmap = std::make_shared<Bitmap>(Bitmap( this ));

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

template<typename T>
T Window::ConfRead( wxString name, T def )
{ 
    return m_config->Read( wxString("Reader/") + name, def ); 
}

static float scale = 1;
void Window::OnMouseMotion( wxMouseEvent &event )
{
    static int y = -1;
    if ( 
        y != -1 
        && event.Dragging() 
        && event.MiddleIsDown() 
        && ! event.RightIsDown() 
        && ! event.LeftIsDown()
        )
    {
        static int step;
        step += y - event.GetPosition().y;
        if ( step > 10 || step < -10)
        {
            if ( m_zoomThread ) 
                m_zoomThread->Delete();
            step = ( step > 0 ) ? 10 : -10;

            float scale = (float(step) / 100);
            // if no modifier(alt,shift,ctrl) pressed
            // only zoom pointed ( with cursor ) bitmap
            if( ! event.HasAnyModifiers() )
            {
                // get bitmap that is pointed
                auto bitmap = m_bitmap->Get( GetViewStart(), event.GetPosition() );
                if ( bitmap )
                {
                    if ( m_fileHandler->IsExist( bitmap->GetIndex() ) )
                    {
                        auto stream = m_fileHandler->Item( bitmap->GetIndex() );
                        wxLogNull nuller;

                        ZoomThread::Zoom( bitmap, stream, scale );
                        AdjustBitmap();
                        AdjustScrollBar();
                        Refresh();
                    }
                }
            }
            // if Ctrl is pressed then zoom all iamges
            // shown in current page
            else if ( event.GetModifiers() == wxMOD_CONTROL )
            {
                if ( m_zoomThread ) 
                {
                    m_zoomThread->SetId(-1);
                    m_zoomThread->Delete();
                }
                m_zoomThread = new ZoomThread( this, wxTHREAD_DETACHED, ZoomThreadID );
                m_zoomThread->SetParameter( m_fileHandler, m_bitmap, scale );
                m_zoomThread->Run();
            }

            step = 0;
        }
    }
    y = event.GetPosition().y;
    event.Skip();
}

void Window::OnEdge( wxDirection direction )
{
    if ( m_bitmap && m_isOpened )
    {
        int step = 0;
        if ( direction == wxUP || direction == wxLEFT )
            step = -1;
        
        if ( direction == wxDOWN || direction == wxRIGHT )
            step = 1;

        BITMAP_PAGES status = m_bitmap->ChangePage( step );
        if ( status == BITMAP_ENDOFPAGE )
            if ( ChangeFolder( step ) )
                status = BITMAP_CHANGEPAGE;
        
        AdjustBitmap();
        AdjustScrollBar();
        
        int x = ( m_isFromRight ) ? GetVirtualSize().GetWidth() : 0;
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

        Refresh();
    }
}

} // namespace reader

}; // namespace fmr