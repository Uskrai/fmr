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

enum VectorPos
{
    VECTOR_BEGIN = 0,
    VECTOR_END = -1
};

template<class T>
class Vector
    : public wxVector<T>
{
    public:
        bool IsExist( size_t index )
        {
            return index >= 0  && index < this->item.size();
        }

        int Add ( const T& item, VectorPos position )
        {
            switch ( position )
            {
                case VECTOR_BEGIN:
                    this->insert( this->begin(),item);
                    return VECTOR_BEGIN;
                    break;
                case VECTOR_END:
                    this->push_back(item);
                    return size() - 1;
                    break;
                default:
                {
                    int i = 0;
                    for ( auto it = this->begin(); it != this->end(); ++it )
                    {
                        if ( i == position )
                        {
                            this->insert(  it, item );
                            return i;
                        }
                        i++;
                    }
                }
                return -1;
            }
        }
        using wxVector<T>::size;

};

#endif
