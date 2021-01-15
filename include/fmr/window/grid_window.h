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

#ifndef FMR_WINDOW_GRID_WINDOW
#define FMR_WINDOW_GRID_WINDOW

#include <fmr/window/scrolledwindow.h>
#include <wx/sizer.h>

namespace fmr
{

class GridWindow
    : public ScrolledWindow
{
    protected:
        wxGridSizer *sizer_;

    public:
        GridWindow(
            wxWindow *parent,
            wxWindowID id,
            const wxPoint &pos = wxDefaultPosition,
            const wxSize &size = wxDefaultSize,
            long style = 0,
            const wxString &name = wxPanelNameStr
        );

        bool Create(
            wxWindow *parent,
            wxWindowID id,
            const wxPoint &pos = wxDefaultPosition,
            const wxSize &size = wxDefaultSize,
            long style = 0,
            const wxString &name = wxPanelNameStr
        );


        bool CreateGrid( int rows = 0, int cols = 0, const wxSize &gap = wxDefaultSize );

    private:
        void OnKeyDown( wxKeyEvent &event );
        void OnPaint( wxPaintEvent &event );
        wxDECLARE_EVENT_TABLE();
};


}; // namespace fmr

#endif
