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

#ifndef MyFrame
#define MyFrame

#include <wx/menu.h>
#include <wx/frame.h>
#include <wx/filedlg.h>
#include <wx/sizer.h>
#ifndef WX_PRECOMP
    #include <wx/wxprec.h>
#endif

#include "gui/panel.h"

enum
{
    ID_Hello = 1
};

class Frame :
    public wxFrame
{
    public :
        Frame(const wxString& title, const wxPoint& pos, const wxSize& size,long style);
        
        Panel* panel = NULL; 
        wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);

    private:
        void SetPanel();

        //event
        void OnHello(wxCommandEvent& event);
        void OnExit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);

        void OpenFile(wxCommandEvent& event);

        wxStatusBar* StatusBar();

        wxMenuBar* MenuBar();
        wxMenu* MenuFile();
        wxMenu* MenuHelp();

        wxDECLARE_EVENT_TABLE();

};

#endif