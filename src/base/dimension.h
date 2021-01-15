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
#ifndef FMR_BASE_DIMENSION
#define FMR_BASE_DIMENSION

#include <wx/position.h>

namespace dimension
{
    inline int Get( const wxPoint &pos, const wxOrientation &orient )
    {
        if ( orient == wxVERTICAL )
            return pos.y;
        else if ( orient == wxHORIZONTAL )
            return pos.x;

        return wxDefaultCoord;
    }

    inline void Set( wxPoint &pt, int orientation, int pos )
    {
        if ( pos == wxDefaultCoord )
            return;

        if ( orientation == wxVERTICAL )
            pt.y = pos;
        if ( orientation == wxHORIZONTAL )
            pt.x = pos;
    }

    inline int Get( const wxSize &size, const wxOrientation &orient )
    {
        wxPoint pos( size.GetWidth(), size.GetHeight() );
        return Get( pos, orient );
    }

    template<class T>
    inline void print( const T &dimension, const char *text )
    {
        printf("%s %dx%d\n", text, dimension.x, dimension.y );
    }

    inline wxDirection GetDirection( const wxOrientation &orient, int pos )
    {
        if ( pos == 0 )
            return wxALL;

        if ( orient == wxVERTICAL )
            return pos < 0 ? wxUP : wxDOWN;
        else if ( orient == wxHORIZONTAL )
            return pos < 0 ? wxLEFT : wxRIGHT;

        return wxALL;
    }

    inline wxOrientation GetOrient( const int &orient )
    {
        if ( orient == wxVERTICAL )
            return wxVERTICAL;
        if ( orient == wxHORIZONTAL )
            return wxHORIZONTAL;
        return wxORIENTATION_MASK;
    }

    inline wxPoint Make( const wxPoint &pt, wxOrientation orient, int pos )
    {
        wxPoint temp( pt );
        dimension::Set( temp, orient, pos );
        return temp;
    }

    // wxDirection GetDirection( const wxPoint &pos, const wxOrientation &orient )
    // {
        // return 
    // }
}

#endif
