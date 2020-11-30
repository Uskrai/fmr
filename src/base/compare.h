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

#include <wx/arrstr.h>

#ifndef FMR_BASE_COMPARE
#define FMR_BASE_COMPARE

#define StringCompareFunction( name ) int name ( const wxString& s1, const wxString& s2 )

bool NotEnd( const wxString& str, size_t idx ) 
    { return idx < str.size(); }

namespace Compare
{
    wxString GetNonZero( const wxString& str, size_t& idx, char& chr )
    {
        size_t sz = idx;
        while ( isdigit( str[idx + 1] ) )
            idx++;

        while ( str[ sz ] == '0' )
            sz++;

        chr = str[idx];
        const wxString& tmp = str.SubString( sz, idx );
        return ( tmp == "" ) ? "0" : tmp;
    }

    StringCompareFunction( Natural ) 
    {
        const wxString& first = s1.Lower();
        const wxString& second = s2.Lower();
        // idx for first, pos for second;
        size_t idx = 0, pos = 0;
        while ( NotEnd( first, idx ) && NotEnd( second, pos ) )
        {
            char fst = first[idx];
            char scnd = second[pos];
            
            if ( isdigit(fst) && isdigit(scnd) )
            {
                const wxString& str1 = GetNonZero( first, idx, fst );
                const wxString& str2 = GetNonZero( second, pos, scnd );

                // if str1's len not equal str2's len
                // will return whether str1 more or less than str2
                if ( str1.size() != str2.size() ) 
                    return str1.size() - str2.size();

                size_t i = 0;
                // only check str1 cuz the len is the same
                while ( NotEnd(str1,i) )
                {
                    // will return whether str1 more or less
                    // than str2 if the value not equal
                    if ( str1[i] != str2[i] )
                        return str1[i] - str2[i];
                    i++;
                }
            }

            if ( fst != scnd )
            {
                if ( idx != pos )
                    return idx - pos;
                return  fst - scnd;
            }

            idx++;pos++;
        }

        return 0;
    }
}

#endif