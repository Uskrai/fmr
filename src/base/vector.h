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

#ifndef VECTORHELPER
#define VECTORHELPER

#include <wx/vector.h>
#include <wx/arrstr.h>

namespace Vector
{
    template< typename T>
    bool IsExist( const wxVector<T>& vec, size_t idx )
    {
        return idx < vec.size();
    }

    bool IsExist( const wxArrayString vec, size_t idx )
    {
        return idx < vec.size();
    }
};

#endif
