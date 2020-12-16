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

#include <wx/sizer.h>

namespace fmr
{

Panel::Panel( wxWindow* parent, wxWindowID id, wxPoint position, wxSize size ) :
    wxPanel( parent, id, position, size )
{
    this->sizer = new wxBoxSizer( wxHORIZONTAL );
    this->m_reader = new reader::Window( this, ReaderWindow, wxDefaultPosition, GetClientSize(), 0, "Reader" );    
    if ( ! m_reader->IsTransparentBackgroundSupported() )
        m_reader->SetBackgroundColour( *wxBLACK );
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
    if ( m_reader )
        m_reader->Destroy();
    return wxPanel::Destroy();
}

}; // namespace fmr