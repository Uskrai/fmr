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
#include "explorer/load_explorer.h"
#include "explorer/image_window_explorer.h"
#include "handler/abstract_handler.h"
#include "handler/handler_factory.h"
#include <vector>

class wxPanel;

namespace fmr
{

namespace explorer
{

enum ThreadID
{
    LoadThread = wxID_HIGHEST + 40
};

class Window 
    : public wxGrid
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

        wxThread *GetThread( int id );
        void DoSetNull( int id );
    
        bool Destroy();
    protected:
        AbstractHandler *handler_;
        Load *load_thread_ = NULL;
        std::vector<ImageWindow*> list_panel_;
        std::vector<SStream> list_stream_;

        void OnThreadUpdate( wxCommandEvent &event );

        wxDECLARE_EVENT_TABLE();
};


}; // namespace Explorer

}; // namespace fmr
#endif