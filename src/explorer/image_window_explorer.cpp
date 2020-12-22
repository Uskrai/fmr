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

#include "explorer/image_window_explorer.h"
#include "bitmap/bmp.h"
#include <wx/dc.h>

namespace fmr
{

namespace explorer
{

ImageWindow::ImageWindow( StreamBitmap *stream_bitmap )
{
    SetStreamBitmap( stream_bitmap );
}

ImageWindow::ImageWindow( const ImageWindow &other )
{
    SetStreamBitmap( other.stream_bitmap_ );
}

void ImageWindow::SetStreamBitmap( StreamBitmap *stream_bitmap )
{
    stream_bitmap_ = stream_bitmap;
}

void ImageWindow::Draw( wxGrid &grid, wxGridCellAttr &attr, wxDC &dc, const wxRect &rect, int row, int col, bool isSelected )
{
    // SBitmap bmp;
    // if ( stream_ )
        // dc.DrawLabel( stream_->GetName() , bmp.GetBitmap(), rect );
}

wxSize ImageWindow::GetBestSize( wxGrid &grid, wxGridCellAttr &attr, wxDC &dc, int row, int col )
{
    return wxSize(300,300);
}

}; // namespace explorer

}; // namespace fmr