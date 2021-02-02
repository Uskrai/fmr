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

#ifndef FMR_READER_PAGE_INDICATOR
#define FMR_READER_PAGE_INDICATOR

#include "fmr/window/decorator.h"

namespace fmr {

namespace reader {

class PageIndicator : public WindowDecorator {
  size_t page_pos_ = 0, page_limit_ = 0;
  wxRect rect_;

 public:
  PageIndicator(){};

  virtual void Draw(wxDC &dc);

  void SetPage(int pos) { page_pos_ = pos; };
  void SetPageLimit(int size) { page_limit_ = size; }

  void SetRect(const wxPoint &pos, const wxSize &size) {
    rect_ = wxRect(pos, size);
  }
};

}  // namespace reader

}  // namespace fmr

#endif /* end of include guard: FMR_READER_PAGE_INDICATOR */
