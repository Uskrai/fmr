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

#include "bitmap/size.h"

#define FitOneSide( left, right)                    \
        wxSize size = parent->GetClientSize();      \
        wxSize imgsz = img.GetSize();               \
        parent->ShowScrollbars( wxSHOW_SB_DEFAULT , \
                                wxSHOW_SB_DEFAULT );\
        float scl = float(scale) / 100;             \
                                                    \
        size.Scale(scl,scl);                        \
        scl = float( size.left)                     \
                    / float( imgsz.left );          

namespace Size
{
    SizeFunc( FitWidth )
    {
        FitOneSide(x,y)
        if ( size.GetWidth() < img.GetWidth() || ( flags & BITMAP_ALLOWENLARGE ) )
        {
            img.Rescale( size.x, imgsz.y * scl );
            return scl;
        }
        return 1;
    }

    SizeFunc ( FitHeight )
    {
        FitOneSide(y,x);
        if ( size.GetHeight() < img.GetWidth() || flags & BITMAP_ALLOWENLARGE )
        {
            img.Rescale( imgsz.x * scl, size.y );
            return scl;
        }
        return scl;
    }
    
    SizeFunc( FitAll )
    {
        wxSize imgsz = img.GetSize();

        if ( imgsz.GetHeight() < img.GetWidth() )
            return FitWidth( img, flags, parent, scale);
        else 
            return FitHeight( img, flags, parent, scale );
    }     
    
    SizeFunc ( Prepare )
    {
        SizeFlagFilter( BITMAP_ORIGINAL, Original)
        else SizeFlagFilter( BITMAP_FITALL, FitAll )
        else SizeFlagFilter( BITMAP_FITWIDTH, FitWidth )
        else SizeFlagFilter( BITMAP_FITHEIGHT, FitHeight )
        return 0;
    }

  
};