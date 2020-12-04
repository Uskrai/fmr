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
        float scl = float(scale) / 100;             \
                                                    \
        size.Scale(scl,scl);                        \
        scl = float( size.left)                     \
                    / float( imgsz.left );          

namespace Size
{
    // bool IsScrollbarShown( wxScrolledWindow* parent, wxSize size, int flags )
    // {
    //     wxSize virori = parent->GetVirtualSize();
    //     wxSize ori = parent->GetClientSize();
    //     parent->SetVirtualSize( size );

    //     wxSize clt = parent->GetClientSize();

    //     parent->SetVirtualSize( virori );
    //     if ( flags & BITMAP_FITHEIGHT ) 
    //         return ori.GetHeight() != clt.GetHeight();

    //     else if ( flags & BITMAP_FITWIDTH )
    //         return ori.GetWidth() != clt.GetWidth();
    // }

    SizeFunc( FitWidth )
    {
        FitOneSide(x,y)

        bool isRescale = size.GetWidth() < img.GetWidth() || flags & BITMAP_ALLOWENLARGE;

        if ( isRescale )
        {
            imgsz.Set( size.x, imgsz.y * scl );
        }

        // if ( IsScrollbarShown( parent, imgsz, flags ) )
        // {
        //     // parent->ShowScrollbars( wxSHOW_SB_DEFAULT, wxSHOW_SB_ALWAYS );
        //     float res = FitWidth( img, flags, parent, scale );
        //     // parent->ShowScrollbars( wxSHOW_SB_DEFAULT, wxSHOW_SB_DEFAULT );
        //     return res;
        // }

        if ( isRescale )
        {
            img.Rescale( imgsz.x, imgsz.y );
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