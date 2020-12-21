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
    config_ = Config::Get();
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
}

void Window::OnDraw( wxDC &dc )
{   
    dc.SetClippingRegion( GetViewStart(), GetClientSize() );
    wxCriticalSectionLocker locker( g_sLock );
    if ( bitmap_ )
    {
        wxVector<SBitmap*> vec = bitmap_->Get( );
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
    if ( bitmap_ )
        AdjustBitmap();
    else 
        SetVirtualSize( sz );
    
    Refresh();
    event.Skip();
}

void Window::AdjustBitmap()
{
    wxCriticalSectionLocker locker(g_sLock);
    if ( bitmap_ )
    {
        bitmap_->Refresh();
        wxSize sz = bitmap_->GetSize( GetClientSize() );
        bitmap_->RefreshPosition( sz );
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

void Window::OnThreadUpdate( wxCommandEvent &event )
{
    AdjustBitmap();
    Refresh();
}

bool Window::Open( const wxString& path )
{
    is_opened_ = false;
    auto open_bitmap = NewBitmap();

    controller_.SetBitmap( open_bitmap );
    ControllerExit open_status = controller_.Open( path );

    if ( open_status == kControllerSuccess )
    {
        ReloadConfig();
        bitmap_ = controller_.GetBitmap();
        file_handler_ = controller_.GetHandler();
        AdjustBitmap();
        Scroll(0,0);
        Refresh();

        AdjustScrollBar();
        config_->Write( "RecentlyOpened", path );
        config_->Flush();
        is_opened_ = true;
        return true;
    }

    controller_.SetBitmap( bitmap_ );
    controller_.SetHandler( file_handler_ );

    if ( open_status == kControllerCantRunThread )
        { wxLogError( "Can't Create a Thread"); }

    if ( open_status == kControllerFolderEmpty )
        { wxLogStatus("%ls doesn't contain any images", path ); }

    if ( file_handler_ )
        { is_opened_ = true; }

    return false;
}

std::shared_ptr<AbstractHandler> Window::NewHandler( const wxString &path )
{
    auto handler = std::shared_ptr<AbstractHandler>(
            HandlerFactory::NewHandler(path)
    );

    if ( handler )
    {
        handler->Open(path);
        handler->Traverse( true );
        if ( handler->GetParent() )
            handler->GetParent()->Traverse();
    }
    
    return handler;
}

std::shared_ptr<Bitmap> Window::NewBitmap()
{
    std::shared_ptr<Bitmap> bitmap;
    bitmap = std::make_shared<Bitmap>(Bitmap( this ));

    size_t image_show_limit = ConfRead("ImageShowLimit", 1 );
    int scale = ConfRead("ImageScaleFromOriginal", 100 );
    long pos = ConfRead("ImagePosition", long(BITMAP_VERTICAL | BITMAP_CENTERED) );
    long sizeflag = ConfRead("ImageSize", long(BITMAP_ORIGINAL) );
    bitmap->SetFlags(pos,sizeflag,scale);
    bitmap->SetLimit( image_show_limit );

    return bitmap;
}

void Window::ReloadConfig()
{
    config_->Flush();
}

bool Window::ChangeFolder( int step )
{
    wxString path = file_handler_->GetFromCurrent( step );

    if ( path == wxEmptyString ) return false;

    if ( Open( path ) ) return true;

    return ChangeFolder( step + step );

}

void Window::Next() { Open( GetHandler()->GetNext()); }
void Window::Prev() { Open( GetHandler()->GetPrev()); }

template<typename T>
T Window::ConfRead( wxString name, T def )
{ 
    return config_->Read( wxString("Reader/") + name, def ); 
}

// static float scale = 1;
void Window::OnMouseMotion( wxMouseEvent &event )
{
    // static int y = -1;
    // if ( 
    //     y != -1 
    //     && event.Dragging() 
    //     && event.MiddleIsDown() 
    //     && ! event.RightIsDown() 
    //     && ! event.LeftIsDown()
    //     )
    // {
    //     static int step;
    //     step += y - event.GetPosition().y;
    //     if ( step > 10 || step < -10)
    //     {
    //         if ( m_zoomThread ) 
    //             m_zoomThread->Delete();
    //         step = ( step > 0 ) ? 10 : -10;

    //         float scale = (float(step) / 100);
    //         // if no modifier(alt,shift,ctrl) pressed
    //         // only zoom pointed ( with cursor ) bitmap
    //         if( ! event.HasAnyModifiers() )
    //         {
    //             // get bitmap that is pointed
    //             auto bitmap = bitmap_->Get( GetViewStart(), event.GetPosition() );
    //             if ( bitmap )
    //             {
    //                 if ( file_handler_->IsExist( bitmap->GetIndex() ) )
    //                 {
    //                     auto stream = file_handler_->Item( bitmap->GetIndex() );
    //                     wxLogNull nuller;

    //                     ZoomThread::Zoom( bitmap, stream, scale );
    //                     AdjustBitmap();
    //                     AdjustScrollBar();
    //                     Refresh();
    //                 }
    //             }
    //         }
    //         // if Ctrl is pressed then zoom all iamges
    //         // shown in current page
    //         else if ( event.GetModifiers() == wxMOD_CONTROL )
    //         {
    //             if ( m_zoomThread ) 
    //             {
    //                 m_zoomThread->SetId(-1);
    //                 m_zoomThread->Delete();
    //             }
    //             // m_zoomThread = new ZoomThread( this, wxTHREAD_DETACHED, ZoomThreadID );
    //             // m_zoomThread->SetParameter( file_handler_, bitmap_, scale );
    //             // m_zoomThread->Run();
    //         }

    //         step = 0;
    //     }
    // }
    // y = event.GetPosition().y;
    event.Skip();
}

void Window::OnEdge( wxDirection direction )
{
    if ( bitmap_ && is_opened_ )
    {
        int step = 0;
        if ( direction == wxUP || direction == wxLEFT )
            step = -1;
        
        if ( direction == wxDOWN || direction == wxRIGHT )
            step = 1;

        BITMAP_PAGES status = bitmap_->ChangePage( step );
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