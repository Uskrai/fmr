/*
 *  Copyright (c) 2021 Uskrai
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

#ifndef FMR_GRID_CELL_WINDOW
#define FMR_GRID_CELL_WINDOW

#include <wx/window.h>
#include <wx/dcclient.h>
#include <wx/sizer.h>

namespace fmr
{

class FlexGridCellWindow
    : public wxWindow
{
    protected:
        wxWindow *cell_window_ = NULL;
        wxSizer *sizer_ = NULL;
        bool is_selected_ = false;
        int border_width_ = 0, highlight_width_ = 0;

    public:
        FlexGridCellWindow(
            wxWindow *parent,
            wxWindowID id
        );

        bool Create(
            wxWindow *parent,
            wxWindowID id
        );

        wxSize GetMinSize() const override;

        void SetSelected( bool is_selected = true )
        { is_selected_ = is_selected; }

        bool AcceptsFocus() const override
        { return false; }

        void SetCellWindow( wxWindow *cell_window );

        wxWindow *GetCellWindow()
        { return cell_window_; }

        void SetBorderWidth( int border )
        { border_width_ = border; }

        int GetBorderWidth() const
        { return border_width_; }

        void SetHighlightPenWidth( int width )
        { highlight_width_ = width; }

        int GetHighlightPenWidth() const
        { return highlight_width_; }

        wxRect GetCellRect() const;

    private:
        wxDECLARE_EVENT_TABLE();

        void OnPaint( wxPaintEvent &event );
        void OnSize( wxSizeEvent &event );
        void DrawBorder( wxDC &dc );
};

} // namespace fmr end

#endif // FMR_GRID_CELL_WINDOW end

