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

#ifndef FMR_EXPLORER_STRING_DRAW
#define FMR_EXPLORER_STRING_DRAW

#include <fmr/common/rect.h>
#include <fmr/explorer/string_rect.h>
#include <wx/gdicmn.h>

#include <string>
#include <vector>

class wxDC;

namespace fmr {

namespace explorer {

class StringDraw : public Rectangle {
  std::string text_;
  std::vector<StringRect> splitted_text_;
  bool should_prepare_ = true;
  int ver_flags_ = 0, hor_flags_ = 0;

 public:
  StringDraw() {}
  StringDraw(const std::string &string) { SetString(string); }

  void SetString(const std::string &string) {
    text_ = string;
    Prepare();
  }
  std::string GetString() const { return text_; }

  std::vector<StringRect> SplitString(wxDC &dc);

  void PrepareString(wxDC &dc);
  void Prepare(bool cond = true) { should_prepare_ = cond; }
  bool ShouldPrepare() const { return should_prepare_; }

  void SetRect(const wxRect &rect) override {
    Rectangle::SetRect(rect);
    Prepare();
  }

  /**
   * @brief: Flags passed to dimension::AlignPosition
   */
  void SetHorizontalFlags(int flags) {
    hor_flags_ = flags;
    Prepare();
  }

  /**
   * @brief: Flags passed to dimension::AlignPosition
   */
  void SetVerticalFlags(int flags) {
    ver_flags_ = flags;
    Prepare();
  }

  void Draw(wxDC &dc);
};

}  // namespace explorer

}  // namespace fmr

#endif /* end of include guard: FMR_EXPLORER_STRING_DRAW */
