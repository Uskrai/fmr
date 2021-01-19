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

#ifndef FMR_GUI_PANEL
#define FMR_GUI_PANEL

#include <fmr/reader/window_reader.h>
#include <fmr/explorer/window_explorer.h>
#include <fmr/common/string.h>

#include <wx/panel.h>

class wxBoxSizer;

namespace fmr
{

enum WindowID
{
    ReaderWindow = wxID_HIGHEST + 1,
    ExplorerWindow
};

class Panel :
    public wxPanel
{
    public: 
        Panel( wxWindow* parent, wxWindowID id, wxPoint position, wxSize size ); 

        bool LoadFile( std::string path );
        bool OpenExplorer();
        bool Destroy();
    private:
        void BindEvent();

        void OnKeyDown( wxKeyEvent &event );
        void OnCharHook( wxKeyEvent &event );
        void OnExplorerOpenFile( wxCommandEvent &event );
        reader::Window* reader_ = NULL;
        explorer::Window *explorer_ = NULL;
        wxBoxSizer* sizer_;

};

}; // namespace fmr

#endif
