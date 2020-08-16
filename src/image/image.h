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

#ifndef READER_IMAGE
#define READER_IMAGE

#include "base/vector.h"
#include <wx/image.h>

class wxScrolledWindow;

class Image
{
    public:
        Image() {}
        ~Image();

        wxVector<wxImage>& Get() { return m_item; }

        void Clear(); // clear all value

        bool Load( wxInputStream* stream, int pos );
    private:
        wxVector<wxImage> m_item;
};

#endif
