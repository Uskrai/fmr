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

void Reader::LoadImage( wxString path )
{
    this->image->Open(path);
}

void Reader::OnDraw( wxDC& dc )
{
    for ( const auto& it : this->image->Get() )
    {
        dc.DrawBitmap( *(it), 0,0 );
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

Reader::~Reader()
{
    delete this->image;
}

} // end of namespace Reader