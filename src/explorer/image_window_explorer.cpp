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

    refresh_scheduled_ = true;
}

void ImageWindow::SetBitmap( std::shared_ptr<SBitmap> bmp )
{
    bitmap_ = bmp;
    refresh_scheduled_ = true;
}

void ImageWindow::SetStream( std::shared_ptr<SStream> stream )
{
    stream_ = stream;
    refresh_scheduled_ = true;
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

std::vector<StringDraw> SplitString( std::string string, const wxSize &size, wxDC &dc )
{
    std::vector<StringDraw> list_string;

    StringDraw string_draw;

    std::string per_space_string, per_char_string;

    for ( const auto &it : string )
    {
        per_char_string += it;

        wxString string_to_check;
        String::FromUTF8( per_space_string + per_char_string, string_to_check );

        wxSize text_extent = dc.GetTextExtent( string_to_check );

        if ( text_extent.GetWidth() > size.GetWidth() )
        {
            string_draw.filename = per_space_string;

            list_string.push_back( string_draw );

            per_space_string = "";
        }

        if ( it != ' ' )
            continue;

        per_space_string += per_char_string;
        per_char_string = "";
    }

    per_space_string +=  per_char_string;
    string_draw.filename = per_space_string;

    list_string.push_back( string_draw );

    return list_string;
}

void ImageWindow::PrepareRect( const wxRect &rect )
{
    bitmap_rect_ = rect;
    text_rect_ = rect;

    bitmap_rect_.SetSize( GetBestBitmapSize( rect.GetSize() ) );

    wxSize txt_size = text_rect_.GetSize();
    txt_size.Scale( 1, 0.2 );
    text_rect_.SetSize( txt_size );
    text_rect_.SetTop( bitmap_rect_.GetBottom() + 5 );

    if ( bitmap_ && bitmap_->IsOk() )
    {
        wxRect bmp_rect = wxRect( bitmap_->GetPosition(), bitmap_->GetSize() );
        bmp_rect = bmp_rect.CenterIn( bitmap_rect_ );
        bitmap_position_ = bmp_rect.GetPosition();

        bitmap_size_ = bitmap_->GetSize();
    }

    this_rect_ = rect;
}

void ImageWindow::PrepareStringPos( wxDC &dc, const wxRect &rect )
{
    if ( !stream_ )
        return;

    vec_string_draw_ = SplitString(
        Path::GetName( stream_->GetName() ),
        text_rect_.GetSize(),
        dc
    );

    wxPoint text_pos = text_rect_.GetTopLeft();
    wxRect string_rect;

    for ( auto &it : vec_string_draw_ )
    {
        wxString string;
        String::FromUTF8( it.filename, string );

        it.rect.SetSize( dc.GetTextExtent( string ) );
        it.rect = it.rect.CenterIn( text_rect_ );
        it.rect.SetTop( text_pos.y );

        text_pos = it.rect.GetBottomLeft();
    }

    string_name_ = stream_->GetName();
}

void ImageWindow::Draw( wxGrid &grid, wxGridCellAttr &attr, wxDC &dc, const wxRect &rect, int row, int col, bool isSelected )
{
    if ( stream_ && string_name_ != stream_->GetName() )
        refresh_scheduled_ = true;

    if ( bitmap_ && bitmap_size_ != bitmap_->GetSize() )
        refresh_scheduled_ = true;

    if ( this_rect_ != rect || refresh_scheduled_ )
    {
        PrepareRect( rect );
        PrepareStringPos( dc, text_rect_ );
        refresh_scheduled_ = false;
    }

    if ( bitmap_->IsOk() )
        dc.DrawBitmap( bitmap_->GetBitmap(), bitmap_position_ );


    for ( const auto & it : vec_string_draw_ )
    {
        wxString string;
        String::FromUTF8( it.filename, string );
        dc.DrawText( string , it.rect.GetPosition() );
    }
}

wxSize ImageWindow::GetBestSize( wxGrid &grid, wxGridCellAttr &attr, wxDC &dc, int row, int col )
{
    return wxSize(300,300);
}

}; // namespace explorer

}; // namespace fmr