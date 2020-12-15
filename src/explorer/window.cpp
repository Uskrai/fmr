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

#include "explorer/window.h"
#include <wx/panel.h>

#include "handler/handler.h"
#include "handler/handlerfactory.h"

namespace Explorer
{

Window::Window( 
        wxWindow *parent, 
        const wxWindowID &id,
        const wxPoint &pos,
        const wxSize &size,
        const long &style,
        const wxString &name
)   : ScrolledWindow( parent, id, pos, size, style, name)
{
}

void Window::Open( const wxString &name )
{
    m_fileHandler = HandlerFactory::NewHandler( name ); 
    m_fileHandler->Traverse();
    for ( const auto &it : m_fileHandler->GetChild() )
        m_panel.push_back( new wxPanel( this, wxID_ANY ));
}


}