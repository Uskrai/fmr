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

#include <wx/panel.h>

class wxBoxSizer;

enum WindowID
{
    ReaderWindow = wxID_HIGHEST + 1
};
namespace Reader{
    class Window;
};

class Panel :
    public wxPanel
{
    public: 
        Panel( wxWindow* parent, wxWindowID id, wxPoint position, wxSize size ); 

        void LoadFile( wxString path );
        bool Destroy();
    private:
        Reader::Window* m_reader = NULL;
        wxBoxSizer* sizer;

};