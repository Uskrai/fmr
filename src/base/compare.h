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

namespace fmr
{


#define DeclareStringCompareFunction( name ) \
    bool name ## Sortable ( const Sortable &s1, const Sortable &s2 );    \
    bool name ## String ( const std::wstring &s1, const std::wstring &s2 )

#define DefineStringCompareFunction( name ) \
    bool name ## Sortable ( const Sortable &s1, const Sortable &s2 ) \
        { return name ## String ( s1.GetString(), s2.GetString() ); }; \
    bool name ## String ( const std::wstring &s1, const std::wstring &s2 )

    
namespace Compare
{
    typedef wchar_t Char;
    typedef std::wstring String;

    class Sortable 
    {
        public :
            virtual String GetString() const = 0;
    };

    bool NotEnd( const wxString& str, size_t idx );
    String GetNonZero( const String& str, size_t& idx, Char& chr );
    DeclareStringCompareFunction( Natural );
}

}; // namespace fmr
#endif