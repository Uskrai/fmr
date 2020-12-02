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

#include "reader/threadreader.h"
#include "base/config.h"

#include "image/image.h"

#include "bitmap/bitmap.h"

#include <wx/stattext.h>
#include <wx/sizer.h>
#include <wx/dcclient.h>

#include "handler/handlerfactory.h"
#include <wx/log.h>

// TODO : Make separate thread controller

namespace Reader
{

#define Free( var )         \
    if ( var )              \
    {                       \
        delete var;         \
        var = NULL;         \
    }

wxBEGIN_EVENT_TABLE( Window, wxWindow )
    EVT_PAINT(Window::OnDraw)
//     EVT_MOTION(Window::OnMouseMotion)
//     EVT_MOUSEWHEEL(Window::OnMouseWheel)
//     EVT_KEY_DOWN(Window::OnKeyDown)
    EVT_COMMAND( wxID_ANY, EVT_COMMAND_LOADTHREAD_UPDATE, Window::OnThreadUpdate )
    EVT_COMMAND( wxID_ANY, EVT_COMMAND_LOADTHREAD_COMPLETED, Window::OnThreadComplete)
wxEND_EVENT_TABLE()

Window::Window( wxWindow* parent, wxWindowID id, const wxPoint & pos, 
                const wxSize &size, long style, const wxString &name ) :
    wxPanel( parent, id, wxDefaultPosition, size, style | wxVSCROLL | wxHSCROLL, name )
{
    m_config = Config::Get();
};

Window::~Window()
{
    if ( m_thread ) m_thread->Delete();
}

void Window::Open( const wxString& path )
{
    // open if path is not empty
    if ( path != wxEmptyString )
    {
        Handler *tempHandler = m_fileHandler;
        Bitmap *tempBitmap = m_bitmap;
        LoadThread *tempThread = m_thread;
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


                Bitmap *bitmap = NewBitmap( handler->Size() );
                size_t idx = handler->Index( path );

                LoadThread *thread = new LoadThread( this );
                thread->SetParameter( bitmap, handler, idx );
                thread->SetLimit( limitPrev, limitNext );

                if ( thread->Run() != wxTHREAD_NO_ERROR )
                {
                    wxLogError("Can't Create Thread");
                    Free(handler)
                    Free(bitmap)
                    Free(thread)
                    return;
                }

                Free(tempHandler);
                Free(tempBitmap);
                Free(tempThread);

                m_fileHandler = handler;
                m_bitmap = bitmap;
                m_thread = thread;
            }
            else
                wxLogStatus( path + " doesn't have any image");
        } // end of if m_filehandler not null
    } // end of if path not empty
}

Handler *Window::NewHandler( const wxString &path )
{
    Handler *handler = HandlerFactory::NewHandler(path);

    if ( handler )
    {
        handler->Open(path);
        handler->Traverse( true );
        if ( handler->GetParent() )
            handler->Traverse();
    }
    
    return handler;
}

Bitmap *Window::NewBitmap( size_t size )
{
    Bitmap *bitmap = NULL;
    bitmap = new Bitmap( this );

    bitmap->SetLimit( size );
    bitmap->Resize( size );
    bitmap->GetAll().assign( size,  SBitmap() );

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

void Window::ChangeFolder( int step )
{
    wxString path = m_fileHandler->GetFromCurrent( step );

    if ( path == wxEmptyString ) return;
    Open ( path );
}

void Window::Next() { Open( GetHandler()->GetNext()); }
void Window::Prev() { Open( GetHandler()->GetPrev()); }

void Window::Find( const wxString& path )
{
}

void Window::Clear()
{
}

// void Window::OnDraw( wxDC& dc )
void Window::OnDraw( wxPaintEvent &event )
{   
    wxPaintDC dc(this);
    // dc.SetClippingRegion( GetViewStart(), GetClientSize() );
    wxCriticalSectionLocker locker( LoadThread::s_GLock );
    if ( m_bitmap )
    {
        wxVector<SBitmap*> vec = m_bitmap->Get();
        for ( const auto& it : vec )
        {
            if ( it->IsOk() )
                dc.DrawBitmap( it->GetBitmap() , it->GetPosition() );
        }
    }
}

template<typename T>
T Window::ConfRead( wxString name, T def )
{ 
    return m_config->Read( wxString("Reader/") + name, def ); 
}

void Window::Error( wxSize size )
{
    wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText* statbox = new wxStaticText( this, wxID_ANY, wxString("Can't load the image"), wxPoint(0,size.GetY() - 22 ), wxSize(size.GetX(),22) ) ;
    statbox->SetBackgroundColour( *wxRED );
    statbox->SetForegroundColour( *wxWHITE );
    sizer->Add(statbox,0, wxALL);
    this->SetSizer(sizer);
}

// void Window::OnMouseWheel( wxMouseEvent& event )
// {
//     int scrolling = event.GetWheelDelta()/event.GetWheelRotation() * ConfRead("WheelInvert",-1);
//     if ( ! OnArrow( wxVERTICAL, scrolling, true ) )
//     {
//         // if ( scrolling > -1 )
//         //     Scroll(0,0);
//         // else
//         //     Scroll(0,GetVirtualSize().GetHeight());
//     }
// }

bool IsKeyDown( wxKeyEvent &event, int val )
{
    return event.GetKeyCode() == val;
}

// void Window::OnKeyDown( wxKeyEvent& event )
// {

//     wxEventType key = event.GetKeyCode();
//     int modVer = ConfRead("ArrowVerticalInvert",-1);
//     int modHor = ConfRead("ArrowHorizontalInvert",-1 );

//     bool IsUp = IsKeyDown( event, WXK_UP );
//     bool IsDown = IsKeyDown( event, WXK_DOWN );
//     bool IsRight = IsKeyDown( event, WXK_RIGHT );
//     bool IsLeft = IsKeyDown( event, WXK_LEFT );

//     if ( IsUp || IsDown || IsRight || IsLeft )
//     {
//         bool isInstant;
//         wxOrientation orient = wxVERTICAL;
//         int modifier = 0;

//         if ( IsUp || IsDown )
//         {
//             orient = wxVERTICAL;
//             modifier = modVer;
//             isInstant = ConfRead("IsInstantOnVertical",false);
//         }
        
//         if ( IsRight || IsLeft )
//         {
//             orient = wxHORIZONTAL;
//             modifier = modHor;
//             isInstant = ConfRead("IsInstantOnHorizontal",true);
//         }

//         // if left or down multiply modifier by -1 to 
//         // make it scroll down or left
//         if ( IsRight || IsDown )
//             modifier *= -1;

//         // if ( OnArrow( orient, modifier, isInstant ) == BITMAP_CHANGEPAGE )
//         //     Scroll(wxPoint(0,0));
        
//         return;
//     }
//     event.Skip();
// }

// BITMAP_PAGES Window::OnArrow( wxOrientation orient, int modifier, bool isInstant )
// {
//     BITMAP_PAGES result = BITMAP_NOTCHANGED;
//     if ( m_thread->IsOpened() || modifier != 0 )
//     {
//         const wxPoint& view = GetViewStart();

//         // get per scroll step
//         int step = ConfRead("ScrollStep", 300 ) * modifier;
//         int ver = step * (orient == wxVERTICAL);
//         int hor = step * (orient == wxHORIZONTAL);

//         Scroll( view + wxPoint( hor , ver ) );

//         static int onEdge;
//         if ( view == GetViewStart() )
//         {
//             onEdge++;
//             result = OnEdge( modifier, isInstant, onEdge );
//             if ( result != BITMAP_NOTCHANGED || result != BITMAP_NOTLOADED )
//             {
//                 onEdge = 0;
//             }
//         } 
//         else onEdge = 0;
//     }    
    
//     return result;
// } 

BITMAP_PAGES Window::OnEdge( int modifier, bool isInstant, int onEdgeCount )
{
    BITMAP_PAGES result = BITMAP_NOTCHANGED;
    size_t conf = ConfRead("ClickBeforeChangePage",1);

    if ( (onEdgeCount > conf || isInstant) && modifier != 0 )
    {
        wxCriticalSectionLocker locker( LoadThread::s_GLock );
        result = m_bitmap->ChangePage(modifier);

        if ( result == BITMAP_ENDOFPAGE )
            ChangeFolder(modifier);
    }
    return result;

}

// void Window::OnMouseMotion( wxMouseEvent& event )
// {
//     static wxPoint mousePosition;
//     const wxPoint& pos = event.GetPosition();
//     if ( event.Dragging() )
//     {
//         if ( event.LeftIsDown() )
//         {
//             int y = mousePosition.y - pos.y;
//             int x = mousePosition.x - pos.x;
//             // const wxPoint& scrolled = GetViewStart();
//             // Scroll( scrolled.x + ( x * 5 ), scrolled.y + ( y  * 5 ));
//         }
//     }
//     mousePosition = pos;
//     event.Skip();
// }

}
