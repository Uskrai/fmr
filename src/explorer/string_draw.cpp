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

#include "fmr/explorer/string_draw.h"

#include <wx/dc.h>

#include "fmr/common/dimension.h"

namespace fmr {

namespace explorer {

void StringDraw::Draw(wxDC &dc) {
  if (ShouldPrepare()) PrepareString(dc);

  wxDCTextColourChanger change(dc, *wxWHITE);
  for (auto &it : splitted_text_) it.Draw(dc);
}

std::vector<StringRect> StringDraw::SplitString(wxDC &dc) {
  std::vector<StringRect> vec;
  wxRect rect = GetRect();

  std::string string_line;
  std::string string_space;

  auto push_string = [&, this](std::string &&text) {
    wxSize text_extent = dc.GetTextExtent(text);
    StringRect str;
    str.SetString(std::move(text));
    str.SetSize(text_extent);

    wxRect str_rect = rect;
    wxPoint pos = dimension::AlignPosition(str_rect, text_extent, hor_flags_);

    str.SetPosition(pos);
    str.SetWidth(rect.GetWidth());
    str.SetHeight(text_extent.GetHeight());

    rect.SetY(rect.GetY() + text_extent.GetHeight());
    vec.push_back(std::move(str));
  };

  for (const auto &it : GetString()) {
    string_space += it;
    if (it != ' ') {
      continue;
    }

    wxSize text_extent = dc.GetTextExtent(string_line + string_space);
    if (text_extent.GetWidth() > GetRect().GetWidth()) {
      push_string(std::move(string_line));
      string_line = "";
    }

    string_line += string_space;
    string_space = "";
  }

  string_line += string_space;
  if (!string_line.empty()) push_string(std::move(string_line));

  return vec;
}

void StringDraw::PrepareString(wxDC &dc) {
  auto vec = SplitString(dc);

  wxRect rect = GetRect();
  wxRect str_rect;
  str_rect.SetWidth(rect.GetWidth());
  str_rect.SetY(rect.GetY());
  str_rect.SetX(rect.GetX());

  for (auto &it : vec) {
    wxRect temp_rect = it.GetRect();
    str_rect.SetHeight(str_rect.GetHeight() + temp_rect.GetHeight());
  }

  wxPoint pos = dimension::AlignPosition(rect, str_rect.GetSize(), ver_flags_);

  for (auto &it : vec) {
    it.SetY(pos.y);
    pos.y += it.GetRect().GetHeight();
  }

  splitted_text_ = vec;
  Prepare(false);
}

void StringRect::PrepareString(wxDC &dc) {
  //

  wxRect rect = GetRect();
  wxSize text_extent_ = dc.GetTextExtent(text_);
}

void StringRect::Draw(wxDC &dc) {
  PrepareString(dc);
  dc.DrawText(text_, GetRect().GetPosition());
  ;
}

}  // namespace explorer

}  // namespace fmr
