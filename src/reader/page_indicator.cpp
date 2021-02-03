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

#include "fmr/reader/page_indicator.h"

#include <wx/dc.h>

#include "fmr/common/dimension.h"

namespace fmr {

namespace reader {

void PageIndicator::OnDraw(wxDC &dc) {
  dc.SetTextForeground(*wxBLACK);
  wxString draw_text = wxString::Format("%ld/%ld", page_pos_, page_limit_);

  wxFont font = *wxNORMAL_FONT;
  font.MakeLarger().MakeLarger();
  dc.SetFont(font);

  wxSize text_size = dc.GetTextExtent(draw_text);
  wxPoint pos = dimension::AlignPosition(
      rect_, text_size, wxALIGN_BOTTOM | wxALIGN_CENTER_HORIZONTAL);

  dc.DrawRectangle(pos, text_size);
  dc.DrawText(draw_text, pos);
  // dc.DrawEllipse(wxPoint(100 + pos.x, 300 + pos.y), wxSize(1000, 1000));
}

}  // namespace reader

}  // namespace fmr
