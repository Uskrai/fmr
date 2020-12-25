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
    stream_ = std::make_shared<SStream>
        ( SStream( *other.stream_ ) );

    bitmap_ = std::make_shared<SBitmap>
        ( SBitmap( *other.bitmap_ ) );
}

void ImageWindow::SetBitmap( std::shared_ptr<SBitmap> bmp )
{
    bitmap_ = bmp;
}

void ImageWindow::SetStream( std::shared_ptr<SStream> stream )
{
    stream_ = stream;
}

const std::shared_ptr<SStream> ImageWindow::GetStream() const
    { return stream_; }

std::shared_ptr<SStream> ImageWindow::GetStream()
    { return stream_; }

const std::shared_ptr<SBitmap> ImageWindow::GetBitmap() const
    { return bitmap_; }

std::shared_ptr<SBitmap> ImageWindow::GetBitmap()
    { return bitmap_; }

wxSize ImageWindow::GetBestBitmapSize( const wxSize &size )
{
    wxSize best_size = size;
    best_size.Scale( 1, 0.8 );
    return best_size;
}

void ImageWindow::Draw( wxGrid &grid, wxGridCellAttr &attr, wxDC &dc, const wxRect &rect, int row, int col, bool isSelected )
{
    wxRect bmp_rect( rect );
    wxRect txt_rect( rect );

    bmp_rect.SetSize( GetBestBitmapSize( rect.GetSize() ) );

    wxSize txt_size = txt_rect.GetSize();
    txt_size.Scale( 1, 0.2 );
    txt_rect.SetSize( txt_size );
    txt_rect.SetTop( bmp_rect.GetBottom() );

    if ( bitmap_->IsOk() )
    {
        wxRect bitmap_rect = wxRect( bitmap_->GetPosition(), bitmap_->GetSize() );
        bitmap_rect = bitmap_rect.CenterIn( bmp_rect );
        dc.DrawBitmap( bitmap_->GetBitmap(), bitmap_rect.GetPosition() );

    }

    if ( stream_ )
    {
        wxString filename = Path::GetName( stream_->GetName() );
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