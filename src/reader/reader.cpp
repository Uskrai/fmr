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

#include "reader/reader.h"

namespace Reader 
{

wxBEGIN_EVENT_TABLE( Reader, wxScrolledWindow )
    EVT_MOTION(Reader::OnMouseMotion)
    EVT_MOUSEWHEEL(Reader::OnMouseWheel)
    EVT_KEY_DOWN(Reader::OnKeyDown)
wxEND_EVENT_TABLE()

Reader::Reader( wxWindow* parent, wxSize size ) :
    wxScrolledWindow( parent, wxID_ANY, wxDefaultPosition, size ) 
{
    this->SetScrollRate(1,1);
    this->SetVirtualSize(-1,-1);
};

Reader::Reader( wxWindow* parent, wxSize size, wxString path ):
    Reader(parent, size)
{
    this->LoadImage(path);
}

Reader::~Reader()
{
    delete this->image;
}

void Reader::LoadImage( wxString path )
{
    this->image->Clear();
    this->image->Open(path);
}

void Reader::OnDraw( wxDC& dc )
{
    dc.SetClippingRegion( this->GetViewStart(), this->GetClientSize() );
    wxCriticalSectionLocker locker( this->image->GetLock() );
    for ( const auto& it : this->image->Get( this->GetViewStart(), this->GetClientSize() ) )
    {
        dc.DrawBitmap( this->image->Get(it), this->image->GetPosition(it) );
    }
}

void Reader::Error( wxSize size )
{
    wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText* statbox = new wxStaticText( this, wxID_ANY, wxString("Can't load the image"), wxPoint(0,size.GetY() - 22 ), wxSize(size.GetX(),22) ) ;
    statbox->SetBackgroundColour( *wxRED );
    statbox->SetForegroundColour( *wxWHITE );
    sizer->Add(statbox,0, wxALL);
    this->SetSizer(sizer);
}

void Reader::OnMouseWheel( wxMouseEvent& event )
{
    int scrolling = event.GetWheelDelta()/event.GetWheelRotation() * this->ConfRead("WheelInvert",-1);
    this->OnArrow( wxVERTICAL, scrolling );
    event.Skip();
}

void Reader::OnKeyDown( wxKeyEvent& event )
{
    wxEventType key = event.GetKeyCode();
    int def = this->config->Read("Invert",-1);
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

void Reader::OnArrow( wxOrientation orient, int modifier  )
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

void Reader::OnMouseMotion( wxMouseEvent& event )
{
    event.Skip();
}
} // end of namespace Reade