/*
 *  Copyright (c) 2021 Uskrai
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

#ifndef FMR_EXPLORER_STRING_RECT
#define FMR_EXPLORER_STRING_RECT

#include <fmr/common/rect.h>

class wxDC;

namespace fmr {

namespace explorer {

class StringRect : public Rectangle {
  wxString string_;
  wxPoint text_pos_;

 public:
  StringRect() {}
  StringRect(const std::string &string) { SetString(string); }

  void SetString(const std::string &string);
  std::string GetString() const;

  void PrepareString(wxDC &dc);
  void Draw(wxDC &dc);
};

}  // namespace explorer

}  // namespace fmr

#endif /* end of include guard: FMR_EXPLORER_STRING_RECT */
