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

#ifndef FMR_BITMAP_POSITION
#define FMR_BITMAP_POSITION

#include "bitmap/bmp.h"  
class wxScrolledWindow;

#define CheckFlags( flag, func )    \
    if ( flags & flag )             \
        { func( vec, parent ); }
        
namespace Position
{
    void RefreshVertical( wxVector<SBitmap*>& vec, wxScrolledWindow *parent);

    void RefreshHorizontal( wxVector<SBitmap*>& vec, wxScrolledWindow *parent);

    void RefreshCentered( wxVector<SBitmap*>& vec, wxScrolledWindow *parent );
    void Refresh( wxVector<SBitmap*>& vec, int flags, wxScrolledWindow *parent )
    {
        CheckFlags( BITMAP_VERTICAL,RefreshVertical )
        else CheckFlags( BITMAP_HORIZONTAL, RefreshHorizontal )
        else CheckFlags( BITMAP_WEBTOON, RefreshVertical )

        CheckFlags( BITMAP_CENTERED, RefreshCentered )
    }
};

#endif