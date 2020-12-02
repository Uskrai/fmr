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

#include "gui/panel.h"
#include "reader/windowreader.h"

#include <wx/sizer.h>

Panel::Panel( wxWindow* parent, wxWindowID id, wxPoint position, wxSize size ) :
    wxPanel( parent, id, position, size )
{
    this->sizer = new wxBoxSizer( wxHORIZONTAL );
    this->m_reader = new Reader::Window( this, ReaderWindow, wxDefaultPosition, GetSize(), 0, "Reader" );    
    this->sizer->Add( m_reader, 1, wxALL | wxEXPAND );
    this->SetSizer( this->sizer );
};

void Panel::LoadFile( wxString path )
{
    this->sizer->Clear();

    m_reader->Open( path );


    this->sizer->Add( m_reader, 1, wxALL | wxEXPAND );
}

bool Panel::Destroy()
{
    m_reader->Destroy();
    return wxPanel::Destroy();
}