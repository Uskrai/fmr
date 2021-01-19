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
#ifndef FMR_BITMAP_SIZE
#define FMR_BITMAP_SIZE

#include <fmr/bitmap/bmp.h>
#include <wx/window.h>

namespace fmr {

#define SizeFlagFilter(flag, func) \
  if (flag & flags) return roundf(func(img, flags, parent, scale) * 100) / 100;

#define SizeFunc(name) \
  float name(wxImage &img, int flags, wxWindow *parent, int scale)

namespace Size {
inline SizeFunc(Original) { return 1; };
SizeFunc(FitAll);
SizeFunc(FitWidth);
SizeFunc(FitHeight);
SizeFunc(Prepare);
};  // namespace Size

};  // namespace fmr
#endif
