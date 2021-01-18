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

#ifndef FMR_COMMON_VECTOR
#define FMR_COMMON_VECTOR

#include "stddef.h"

namespace fmr
{

namespace Vector
{
    template< typename T>
    constexpr bool IsExist( const T& vec, size_t idx )
    {
        return idx < vec.size();
    }

    template<typename T>
    void Push( const T&source, T& destination )
    {
        for ( const auto &it : source )
            destination.push_back( it );
    }
};

}; // namespace fmr

#endif
