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
    EVT_SCROLLWIN_LINEUP(Reader::OnLineUp)
    EVT_SCROLLWIN_LINEDOWN(Reader::OnLineDown)
    EVT_MOUSEWHEEL(Reader::OnMouseWheel)
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
    this->image->Open(path);
}

void Reader::OnDraw( wxDC& dc )
{
    int i=0;
    for ( const auto& it : this->image->Get() )
    {
        dc.DrawBitmap( *(it), this->image->GetPosition(i) );
        i++;
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

void Reader::OnLineChange( wxScrollWinEvent& event, int step )
{
    if ( event.GetOrientation() == wxVERTICAL )
    {
        this->Scroll(-1, this->GetViewStart().y + step );
    }
    else
    {
        this->Scroll( this->GetViewStart().x + step, -1 );
    }
}

void Reader::OnLineUp( wxScrollWinEvent& event )
{
    this->OnLineChange( event, -(this->config->Read("ScrollArrow",300)) );
}

void Reader::OnLineDown( wxScrollWinEvent& event )
{
    this->OnLineChange( event, this->config->Read("ScrollArrow",300) );
}

void Reader::OnMouseWheel( wxMouseEvent& event )
{
    int scrolling = -(event.GetWheelDelta()/event.GetWheelRotation()) * (this->config->Read("ScrollWheel",300)) ;
    this->Scroll( -1, this->GetViewStart().y + scrolling);
    event.Skip();
}

} // end of namespace Reader