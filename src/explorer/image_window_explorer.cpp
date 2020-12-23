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
#include "base/path.h"

namespace fmr
{

namespace explorer
{

ImageWindow::ImageWindow( const StreamBitmap &stream_bitmap )
{
    SetStream( stream_bitmap.stream );
    SetBitmap( stream_bitmap.bitmap );
}

ImageWindow::ImageWindow( const ImageWindow &other )
{
    stream_ = new SStream( other.stream_ );
    bitmap_ = new SBitmap( other.bitmap_ );
}

void ImageWindow::SetBitmap( SBitmap *bmp )
{
    bitmap_ = bmp;
}

void ImageWindow::SetStream( SStream *stream )
{
    stream_ = stream;
}

void ImageWindow::Draw( wxGrid &grid, wxGridCellAttr &attr, wxDC &dc, const wxRect &rect, int row, int col, bool isSelected )
{
    wxRect bmp_rect( rect );
    wxRect txt_rect( rect );

    wxSize bmp_size = bmp_rect.GetSize();
    bmp_size.Scale( 1, 0.8 );
    bmp_rect.SetSize( bmp_size );

    wxSize txt_size = txt_rect.GetSize();
    txt_size.Scale( 1, 0.2 );
    txt_rect.SetSize( txt_size );
    txt_rect.SetPosition( bmp_rect.GetBottomLeft() );

    if ( bitmap_->IsOk() )
    {
        wxRect bitmap_rect = wxRect( bitmap_->GetPosition(), bitmap_->GetSize() );
        bitmap_rect = bitmap_rect.CenterIn( bmp_rect );
        dc.DrawBitmap( bitmap_->GetBitmap(), bitmap_rect.GetPosition() );
    }

    if ( stream_ )
    {
        wxString filename = stream_->GetName().AfterLast( Path::Separator );
        txt_size = dc.GetTextExtent( filename );
        if ( txt_size.GetWidth() > rect.GetWidth() )
        {
            // todo : wrap string
        };
        wxRect filename_rect = wxRect();
        filename_rect.SetSize( txt_size );
        filename_rect = filename_rect.CenterIn( txt_rect );

        dc.DrawText( filename, filename_rect.GetPosition() );

    }

}

wxSize ImageWindow::GetBestSize( wxGrid &grid, wxGridCellAttr &attr, wxDC &dc, int row, int col )
{
    return wxSize(300,300);
}

}; // namespace explorer

}; // namespace fmr