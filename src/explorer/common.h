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

#ifndef FMR_EXPLORER_COMMON
#define FMR_EXPLORER_COMMON

#include "bitmap/bmp.h"
#include "handler/struct_stream.h"
#include <memory>

namespace fmr
{

namespace explorer
{

struct StreamBitmap
{
    std::shared_ptr<SBitmap> bitmap = NULL;
    std::shared_ptr<SStream> stream = NULL;
};

struct StringDraw
{
    wxString filename;
    wxRect rect;
};

}; // namespace explorer

}; // namespace fmr

#endif