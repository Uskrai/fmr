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
#ifndef FMR_BITMAP_SIZE
#define FMR_BITMAP_SIZE

#include "bitmap/bmp.h"
#include <wx/scrolwin.h>

#define SizeFlagFilter( flag, func )    \
    if ( flags & flag )                 \
        return func( img, parent, parentscl );

#define SizeFunc( name )                \
    float name( wxImage &img, wxScrolledWindow *parent, int scale )

namespace Size
{
    SizeFunc( Original ) { return 1;};
    SizeFunc( FitAll );
    SizeFunc( FitWidth );
    SizeFunc( FitHeight );

    float Prepare( wxImage &img, int flags, wxScrolledWindow *parent, int parentscl )
    {
        SizeFlagFilter( BITMAP_ORIGINAL, Original)
        else SizeFlagFilter( BITMAP_FITALL, FitAll )
        else SizeFlagFilter( BITMAP_FITWIDTH, FitWidth )
        else SizeFlagFilter( BITMAP_FITHEIGHT, FitHeight )
        return 0;
    }
};

#endif