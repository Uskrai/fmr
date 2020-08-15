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
#include "handler/archivehandler.h"
#include "handler/defaulthandler.h"
#include "handler/filehandler.h"

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
    SetVirtualSize(-1,-1);
    m_config = Config::Get();

    m_image = new Image(this);
    m_bitmap = new BitmapVertical(this);
    m_factory = new HandlerFactory();
    m_thread = new Thread( this, m_image, m_bitmap );
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
    m_bitmap->Clear();
    m_image->Clear();
    m_thread->Clear();
}

void Window::OnDraw( wxDC& dc )
{
    dc.SetClippingRegion( this->GetViewStart(), this->GetClientSize() );
    wxCriticalSectionLocker locker( this->m_thread->GetLock() );

    for ( const auto& it : m_bitmap->Get() )
    {
        dc.DrawBitmap( it.GetBitmap() , it.GetPosition() );
    }
}

template<typename T>
T Window::ConfRead( wxString name, T def )
{ 
    return this->m_config->Read( wxString("Reader/") + name, def ); 
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
    int scrolling = event.GetWheelDelta()/event.GetWheelRotation() * this->ConfRead("WheelInvert",-1);
    this->OnArrow( wxVERTICAL, scrolling );
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
    default:
    event.Skip();
    }
}

void Window::OnArrow( wxOrientation orient, int modifier  )
{
    switch ( orient )
    {
        case wxVERTICAL:
            this->Scroll( -1, this->GetViewStart().y + ( this->ConfRead("ScrollStep",300) * modifier ) );
            break;
        case wxHORIZONTAL:
            this->Scroll( this->GetViewStart().x + ( this->ConfRead("ScrollStep",300) * modifier ), -1 );
            break;
        default:
            break;
    }
}

void Window::OnMouseMotion( wxMouseEvent& event )
{
    event.Skip();
}

}
