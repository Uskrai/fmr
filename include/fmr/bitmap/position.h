/*
 *  Copyright (c) 2020-2021 Uskrai
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

#ifndef FMR_BITMAP_POSITION
#define FMR_BITMAP_POSITION

#include <fmr/bitmap/bmp.h>
class wxWindow;

namespace fmr {

#define CheckFlags(flag, func) \
  if (flags & flag) {          \
    func(vec, flags, size);    \
  }

#define PositionFunc(name) \
  void name(wxVector<SBitmap*> vec, int flags, wxSize size)

namespace Position {
PositionFunc( Centered );
PositionFunc( Vertical );
PositionFunc( Horizontal );
PositionFunc( Refresh );
};  // namespace Position

}  // namespace fmr

#endif
