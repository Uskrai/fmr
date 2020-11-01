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

#include "bitmap/bitmapvertical.h"

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

    m_bitmap = new BitmapVertical(this);
    m_factory = new HandlerFactory();
    m_thread = new Thread( this, m_bitmap );

    m_bitmap->SetLimit( ConfRead("ImageShowLimit", 1  ) );

    int limitNext = ( ConfRead("ImageShowLimit", 1 ) > ConfRead("ImageMemoryLimitNext", NO_LIMIT ) ) ?
                    ConfRead("ImageShowLimit",1) : ConfRead("ImageMemoryLimitNext", NO_LIMIT );

    m_thread->SetLimit( ConfRead("ImageMemoryLimitPrev", NO_LIMIT ), limitNext );
};

Window::~Window()
{
    delete m_thread;
}

void Window::Open( wxString path )
{
    Clear(); 

    m_factory->Find( path );
    m_thread->SetHandler( m_factory->NewHandler() );

    m_thread->Open( path );
}

void Window::Find( const wxString& path )
{
}

void Window::Clear()
{
    m_thread->Clear();
    m_bitmap->Clear();
    Scroll(0,0);
}

void Window::OnDraw( wxDC& dc )
{
    dc.SetClippingRegion( GetViewStart(), GetClientSize() );
    wxCriticalSectionLocker locker( m_thread->GetLock() );
    int i = 0;
    for ( const auto& it : m_bitmap->Get() )
    {
        if ( it->IsOk() )
        {
            dc.DrawBitmap( it->GetBitmap() , it->GetPosition() );
        }
        i++;
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
    OnArrow( wxVERTICAL, scrolling );
    event.Skip();
}

void Window::OnKeyDown( wxKeyEvent& event )
{
    wxEventType key = event.GetKeyCode();
    int def = this->m_config->Read("Invert",-1);
    switch (key)
    {
        case WXK_UP:
            return this->OnArrow( wxVERTICAL, this->ConfRead("ArrowVerticalInvert",def) * 1 );
            break;
        case WXK_DOWN:
            return this->OnArrow( wxVERTICAL, this->ConfRead("ArrowVerticalInvert",def) * -1 );
            break;
        case WXK_LEFT:
            return this->OnArrow( wxHORIZONTAL, this->ConfRead("ArrowHorizontalInvert",def) * 1 );
            break;
        case WXK_RIGHT:
            return this->OnArrow( wxHORIZONTAL, this->ConfRead("ArrowHorizontalInvert",def) * -1 );
            break;
        default:
            event.Skip();
    }
}

void Window::OnArrow( wxOrientation orient, int modifier )
{
    const wxPoint& view = GetViewStart();
    switch ( orient )
    {
        case wxVERTICAL:
            Scroll( -1, GetViewStart().y + ( ConfRead("ScrollStep",300) * modifier ) );
            break;
        case wxHORIZONTAL:
            Scroll( GetViewStart().x + ( ConfRead("ScrollStep",300) * modifier ), -1 );
            break;
        default:
            break;
    }
    // printf("%d\n", GetViewStart().y );
    if ( view == GetViewStart() )
    {
        OnEdge( modifier );
    }
} 

void Window::OnEdge( int modifier )
{
    if ( modifier > 0 )
    {
        if ( m_bitmap->Next() )
            Scroll(0,0);
        // else
            // Next()
        
    }
    else if ( modifier < 0 )
    {
        if ( m_bitmap->Prev() )
            Scroll( 0, GetVirtualSize().GetHeight() );
        // else
            // Prev()
    }
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
