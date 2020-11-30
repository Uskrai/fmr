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
#include <wx/dc.h>

#include "handler/handlerfactory.h"

namespace Reader
{

wxBEGIN_EVENT_TABLE( Window, wxScrolledWindow )
    EVT_MOTION(Window::OnMouseMotion)
    EVT_MOUSEWHEEL(Window::OnMouseWheel)
    EVT_KEY_DOWN(Window::OnKeyDown)
wxEND_EVENT_TABLE()

Window::Window( wxWindow* parent, wxSize size ) :
    wxScrolledWindow( parent, wxID_ANY, wxDefaultPosition, size )
{
    SetScrollRate(1,1);
    SetVirtualSize(GetSize());

    m_config = Config::Get();

    m_bitmap = new Bitmap(this);
    m_factory = new HandlerFactory();
    m_thread = new Thread( this, m_bitmap );

    int limit = ConfRead("ImageShowLimit", 1 );
    m_bitmap->SetLimit( limit );

    int limitNext = ( ConfRead("ImageShowLimit", 1 ) > ConfRead("ImageMemoryLimitNext", NO_LIMIT ) ) ?
                    ConfRead("ImageShowLimit",1) : ConfRead("ImageMemoryLimitNext", NO_LIMIT );

    m_thread->SetLimit( ConfRead("ImageMemoryLimitPrev", NO_LIMIT ), limitNext );
};

Window::~Window()
{
    delete m_thread;
}

void Window::Open( const wxString& path )
{
    if ( path != wxEmptyString )
    {
        Clear(); 
        m_bitmap->Clear();

        #define GetFileHandler( con )                   \
        if ( con )                                      \
        {                                               \
            m_factory->Find( path );                    \
            if ( m_fileHandler ) delete m_fileHandler;  \
            m_fileHandler = m_factory->NewHandler();    \
        }

        GetFileHandler( !GetHandler() )
        else GetFileHandler( !m_factory->Is( GetHandler()->GetName(),path) )

        SetVirtualSize( GetClientSize() + wxSize(1,0) );

        ReloadConfig();
        m_thread->SetHandler( GetHandler() );

        m_config->Write("RecentlyOpened", path );
        m_thread->Open( path );
        Scroll(0,0);
    }
}

void Window::ReloadConfig()
{
    int scale = ConfRead("ImageScaleFromOriginal", 100 );
    long pos = ConfRead("ImagePosition", long(BITMAP_VERTICAL | BITMAP_CENTERED) );
    long size = ConfRead("ImageSize", long(BITMAP_ORIGINAL) );
    m_bitmap->SetFlags(pos,size,scale);
    m_config->Flush();
}

void Window::ChangeFolder( int step )
{
    wxString path = m_fileHandler->GetFromCurrent( step );

    if ( path == wxEmptyString ) return;
    Open ( path );
}
void Window::Next() { Open( m_thread->GetHandler()->GetNext()); }
void Window::Prev() { Open( m_thread->GetHandler()->GetPrev()); }

void Window::Find( const wxString& path )
{
}

void Window::Clear()
{
    m_thread->Clear();
}

void Window::OnDraw( wxDC& dc )
{
    dc.SetClippingRegion( GetViewStart(), GetClientSize() );
    wxCriticalSectionLocker locker( m_thread->GetLock() );
    for ( const auto& it : m_bitmap->Get() )
    {
        if ( it->IsOk() )
            dc.DrawBitmap( it->GetBitmap() , it->GetPosition() );
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

void Window::OnMouseWheel( wxMouseEvent& event )
{
    int scrolling = event.GetWheelDelta()/event.GetWheelRotation() * ConfRead("WheelInvert",-1);
    if ( ! OnArrow( wxVERTICAL, scrolling, true ) )
    {
        if ( scrolling > -1 )
            Scroll(0,0);
        else
            Scroll(0,GetVirtualSize().GetHeight());
    }
}

bool IsKeyDown( wxKeyEvent &event, int val )
{
    return event.GetKeyCode() == val;
}

void Window::OnKeyDown( wxKeyEvent& event )
{

    wxEventType key = event.GetKeyCode();
    int modVer = ConfRead("ArrowVerticalInvert",-1);
    int modHor = ConfRead("ArrowHorizontalInvert",-1 );

    bool IsUp = IsKeyDown( event, WXK_UP );
    bool IsDown = IsKeyDown( event, WXK_DOWN );
    bool IsRight = IsKeyDown( event, WXK_RIGHT );
    bool IsLeft = IsKeyDown( event, WXK_LEFT );

    if ( IsUp || IsDown || IsRight || IsLeft )
    {
        bool isInstant;
        wxOrientation orient = wxVERTICAL;
        int modifier = 0;

        if ( IsUp || IsDown )
        {
            orient = wxVERTICAL;
            modifier = modVer;
            isInstant = ConfRead("IsInstantOnVertical",false);
        }
        
        if ( IsRight || IsLeft )
        {
            orient = wxHORIZONTAL;
            modifier = modHor;
            isInstant = ConfRead("IsInstantOnHorizontal",true);
        }

        // if left or down multiply modifier by -1 to 
        // make it scroll down or left
        if ( IsRight || IsDown )
            modifier *= -1;

        if ( OnArrow( orient, modifier, isInstant ) == BITMAP_CHANGEPAGE )
            Scroll(wxPoint(0,0));
        
        return;
    }
    event.Skip();
}

BITMAP_PAGES Window::OnArrow( wxOrientation orient, int modifier, bool isInstant )
{
    if ( !m_thread->IsOpened() || modifier == 0 ) return BITMAP_NOTCHANGED;
    
    const wxPoint& view = GetViewStart();

    // get per scroll step
    int step = ConfRead("ScrollStep", 300 ) * modifier;
    int ver = step * (orient == wxVERTICAL);
    int hor = step * (orient == wxHORIZONTAL);

    Scroll( view + wxPoint( hor , ver ) );

    if ( view == GetViewStart() )
    {
        m_onEdge++;
        return OnEdge( modifier, isInstant );
    } 
    m_onEdge = 0;
    
    return BITMAP_NOTCHANGED;
} 

BITMAP_PAGES Window::OnEdge( int modifier, bool isInstant )
{

    size_t conf = ConfRead("ClickBeforeChangePage",1);

    if ( (m_onEdge > conf || isInstant) && modifier != 0 )
    {
        m_onEdge = 0;
        wxCriticalSectionLocker locker(m_thread->GetLock());
        BITMAP_PAGES status = m_bitmap->ChangePage(modifier);

        if ( status == BITMAP_ENDOFPAGE )
            ChangeFolder(modifier);
            
        return status;
    }
    return BITMAP_NOTCHANGED;

}

void Window::OnMouseMotion( wxMouseEvent& event )
{
    const wxPoint& pos = event.GetPosition();
    if ( event.Dragging() )
    {
        if ( event.LeftIsDown() )
        {
            int y = m_mousePosition.y - pos.y;
            int x = m_mousePosition.x - pos.x;
            const wxPoint& scrolled = GetViewStart();
            Scroll( scrolled.x + ( x * 5 ), scrolled.y + ( y  * 5 ));
        }
    }
    m_mousePosition = pos;
    event.Skip();
}

}
