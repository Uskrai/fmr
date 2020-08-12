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

#include "reader/bmp.h"

namespace Reader
{

void Bitmap::OnPrepare( const wxImage& image, wxBitmap& bmp )
{
    bmp = wxBitmap( image );
    
    
}

void Bitmap::OnExit( wxBitmap& bmp )
{

}

void Bitmap::AddPosition( const wxBitmap& bmp )
{

}

void Bitmap::RefreshPosition( const wxBitmap& bmp )
{

}

} // namespace reader