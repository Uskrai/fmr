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

PageIndicator::PageIndicator()
    : font_(*wxNORMAL_FONT),
      background_colour_("DIM GRAY"),
      text_colour_(*wxBLACK) {
  font_.MakeLarger().MakeLarger();
  auto *color = &background_colour_;
  background_colour_.Set(color->Red(), color->Green(), color->Blue(), 150);
}

wxString PageIndicator::GetString() {
  std::string string;
  string += std::to_string(page_pos_) + "/" + std::to_string(page_limit_);
  return string;
}

wxSize PageIndicator::GetSize(wxDC &dc) {
  wxFont temp_font = dc.GetFont();

  dc.SetFont(font_);

  wxSize text_size = dc.GetTextExtent(GetString());
  dc.SetFont(temp_font);
  return text_size;
}

void PageIndicator::OnDraw(wxDC &dc, const wxRect &area) {
  dc.SetTextForeground(text_colour_);
  dc.SetBrush(wxBrush(background_colour_, wxBRUSHSTYLE_SOLID));
  wxString draw_text = GetString();

  dc.SetFont(font_);

  wxSize text_size = GetSize(dc);
  wxPoint pos = dimension::AlignPosition(
      area, text_size, wxALIGN_BOTTOM | wxALIGN_CENTER_HORIZONTAL);

  dc.DrawRectangle(pos, text_size);
  dc.DrawText(draw_text, pos);
}

}  // namespace reader

}  // namespace fmr
