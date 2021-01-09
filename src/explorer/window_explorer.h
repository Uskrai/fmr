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

#include "explorer/common.h"
#include "window/scrolledwindow.h"
#include "explorer/controller_explorer.h"
#include "explorer/image_window_explorer.h"
#include "handler/abstract_handler.h"
#include "window/grid_window.h"
#include <memory>
#include "base/path.h"

class wxPanel;

namespace fmr
{

wxDECLARE_EVENT( EVT_OPEN_FILE, wxCommandEvent );

namespace explorer
{


class Window
    : public GridWindow
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

        bool Open( std::shared_ptr<AbstractOpenableHandler> handler );
        bool Open( const std::string &name );

        void Select( std::string name );

        void Clear();
        bool Destroy();
    protected:
        Controller controller_ = Controller( this );
        std::shared_ptr<AbstractOpenableHandler> handler_;
        std::vector<SBitmap> list_bitmap_;
        std::vector<StreamBitmap> list_item_;
        std::vector<ImageWindow*> list_renderer_;

        // wxGridTableBase *grid_table_;

        // std::vector<wxGridCellCoords> list_cell_pos_;

        void OnThreadUpdate( wxCommandEvent &event );
        void OnGridEnter( wxKeyEvent &event );
        // void OnGridSelect( wxGridEvent &event );

        // wxGridCellCoords selected_cell_;
        wxDECLARE_EVENT_TABLE();
};


}; // namespace Explorer

}; // namespace fmr
#endif