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

#ifndef FMR_EXPLORER_IMAGE_WINDOW
#define FMR_EXPLORER_IMAGE_WINDOW

#include "explorer/common.h"
#include "explorer/load_explorer.h"
#include <wx/grid.h>
#include <memory>

namespace fmr
{

namespace explorer
{

class ImageWindow
    : public wxGridCellRenderer
{
    public:
        ImageWindow(){};
        ImageWindow( StreamBitmap *stream_bitmap );
        ImageWindow( const ImageWindow &other  );

        wxGridCellRenderer *Clone() const { return new ImageWindow( *this ); }
        void SetStreamBitmap( StreamBitmap *stream );

        void Draw( wxGrid &grid, wxGridCellAttr &attr, wxDC &dc, const wxRect &rect, int row, int col, bool isSelected );
        wxSize GetBestSize( wxGrid &grid, wxGridCellAttr &attr, wxDC &dc, int row, int col );
    protected:
        StreamBitmap *stream_bitmap_ = NULL;
        wxGridCellAutoWrapStringRenderer string_wrapper_;
};

}; // namespace fmr

}; // namespace fmr

#endif