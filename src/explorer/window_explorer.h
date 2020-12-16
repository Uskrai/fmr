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

#ifndef FMR_EXPLORER_WINDOW
#define FMR_EXPLORER_WINDOW

#include "window/scrolledwindow.h"
#include "handler/abstract_handler.h"
#include "handler/handler_factory.h"
#include <vector>

class wxPanel;

namespace fmr
{

namespace explorer
{

class Window 
    : public ScrolledWindow
{
    public:
        Window( 
            wxWindow *parent, 
            const wxWindowID &id = wxID_ANY,
            const wxPoint &pos = wxDefaultPosition,
            const wxSize &size = wxDefaultSize,
            const long &style = wxTAB_TRAVERSAL,
            const wxString &name = wxPanelNameStr
        );

        void Open( const wxString &name );
    
    protected:
        AbstractHandler *m_fileHandler;
        std::vector<wxPanel*> m_panel;

};


}; // namespace Explorer

}; // namespace fmr
#endif