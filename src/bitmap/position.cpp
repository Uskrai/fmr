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

#include "bitmap/position.h"
#include <wx/scrolwin.h>

#define RefreshFunc( space )            \
    void Position::Refresh ## space     \
        ( wxVector<SBitmap*>& vec,      \
        wxScrolledWindow *parent  )


#define PositionFunc( coord, dimension )            \
        size_t dimension = 0, pos = 0, temp = 0;    \
        size_t prnt = 0;                            \
        prnt = parent->GetVirtualSize().            \
                Get ## dimension();                 \
        for ( const auto& it : vec )                \
            dimension += it->Get ## dimension();    \
                                                    \
        if (dimension < prnt)                       \
            pos = ( prnt - dimension ) / 2;         \
                                                    \
        for ( auto& it : vec )                      \
        {                                           \
            if ( it->IsOk() )                       \
            {                                       \
                it->Set ## coord ( pos + temp );    \
                pos = it->Get  ## coord ();         \
                temp = it->Get ## dimension ();     \
            }                                       \
        }                                           

RefreshFunc(Vertical)
{
    PositionFunc(Y,Height);
}

RefreshFunc(Centered)
{
    wxSize size = parent->GetClientSize();
    int maxWidth = 0;
    for ( const auto& it : vec )
    {
        if ( it->GetWidth() > maxWidth )
            maxWidth = it->GetWidth();
    }

    for ( const auto& it : vec )
    {
        int width = ( it->GetWidth() < maxWidth ) ? it->GetWidth() : maxWidth;
        int pos = (size.GetWidth() - width) / 2;
        pos = (pos > 0) ? pos : 0;
        it->SetX(pos);
    }
}


RefreshFunc(Horizontal)
{
    PositionFunc(X,Width);
}

