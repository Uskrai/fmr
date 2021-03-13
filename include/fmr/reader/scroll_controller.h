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

#ifndef FMR_READER_SCROLL_CONTROLLER
#define FMR_READER_SCROLL_CONTROLLER

#include "fmr/window/scrolled_image.h"

namespace fmr {

namespace reader {

enum EnumChangePage { ChangeForward = 1, ChangeBackward = -1 };

enum ScrollBitmapFlags {
  kScrollToBitmapRightTop,
  kScrollToBitmapLeftTop,
  kScrollToBitmapLeftBottom,
  kScrollToBitmapRightBottom

};

class ScrollController : public wxEvtHandler {
  ScrolledImageWindow *window_ = nullptr;
  bool is_read_from_right_ = false;

 public:
  ScrollController(){};
  ScrollController(ScrolledImageWindow *window) { SetWindow(window); };
  virtual ~ScrollController() = default;

  void SetWindow(ScrolledImageWindow *window);
  ScrolledImageWindow *GetWindow() { return window_; }
  const ScrolledImageWindow *GetWindow() const { return window_; }

  void Scroll(const wxPoint &pos);
  void Scroll(SBitmap *bitmap, ScrollBitmapFlags flags);
  void ResetScroll(wxDirection direction);

  /**
   * @brief: Get bitmap view start position from flags
   */
  wxPoint GetPosition(const SBitmap *bitmap, ScrollBitmapFlags flags) const;

  ScrollBitmapFlags GetBitmapFlags(int step) const;
  ScrollBitmapFlags GetBitmapFlags(wxDirection direction) const {
    return GetBitmapFlags(GetStep(direction));
  }

  wxPoint GetStartPosition(wxDirection direciion) const;

  bool IsReadFromRight() const { return is_read_from_right_; }
  void SetReadFromRight(bool cond = true) { is_read_from_right_ = cond; }

  void GetFirstShown(const SBitmap *&bitmap, wxPoint *pos = nullptr) const;
  void SetFirstShown(const SBitmap *bitmap, const wxPoint *pos = nullptr);

  int GetStep(wxDirection direction) const;

  bool IsOnEdge(wxOrientation &orient, int position) const;

 private:
  void OnPageChanged(wxCommandEvent &event){};
};

}  // namespace reader

}  // namespace fmr

#endif /* end of include guard: FMR_READER_SCROLL_CONTROLLER */
