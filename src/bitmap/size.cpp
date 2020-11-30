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
        parent->ShowScrollbars( wxSHOW_SB_DEFAULT , wxSHOW_SB_ALWAYS);
        FitOneSide(x,y)
        img = img.Scale( size.x, imgsz.y * scl );
        return scl;
    }

    SizeFunc ( FitHeight )
    {
        parent->ShowScrollbars( wxSHOW_SB_ALWAYS, wxSHOW_SB_DEFAULT );
        FitOneSide(y,x);
        img = img.Scale( imgsz.x * scl, size.y );
        return scl;
    }
    
    SizeFunc( FitAll )
    {
        wxSize imgsz = img.GetSize();

        if ( imgsz.GetHeight() < img.GetWidth() )
            return FitWidth( img, parent, scale );
        else 
            return FitHeight( img, parent, scale );
    }     
};