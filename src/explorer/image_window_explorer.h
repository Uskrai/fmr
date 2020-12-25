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
        ImageWindow( const StreamBitmap &stream_bitmap );
        ImageWindow( const ImageWindow &other  );

        void SetBitmap( std::shared_ptr<SBitmap> bmp );
        void SetStream( std::shared_ptr<SStream> stream );

        const std::shared_ptr<SStream> GetStream() const;
        std::shared_ptr<SStream> GetStream();
        const std::shared_ptr<SBitmap> GetBitmap() const;
        std::shared_ptr<SBitmap> GetBitmap();

        wxGridCellRenderer *Clone() const { return new ImageWindow( *this ); }

        static wxSize GetBestBitmapSize( const wxSize &size );

        void Draw( wxGrid &grid, wxGridCellAttr &attr, wxDC &dc, const wxRect &rect, int row, int col, bool isSelected );
        wxSize GetBestSize( wxGrid &grid, wxGridCellAttr &attr, wxDC &dc, int row, int col );
    protected:
        std::shared_ptr<SStream> stream_ = NULL;
        std::shared_ptr<SBitmap> bitmap_ = NULL;
        wxGridCellAutoWrapStringRenderer string_wrapper_;
};

}; // namespace fmr

}; // namespace fmr

#endif