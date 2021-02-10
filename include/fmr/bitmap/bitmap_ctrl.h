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

#ifndef FMR_BITMAP_BITMAP_CTRL
#define FMR_BITMAP_BITMAP_CTRL

#include <wx/window.h>

#include "fmr/bitmap/bitmap_page.h"
#include "fmr/bitmap/loader.h"
#include "fmr/bitmap/position_ctrl.h"

namespace fmr {

class ScrolledImageWindow;

namespace bitmap {

class BitmapVectorEvent;
wxDECLARE_EVENT(kEventBitmapChanging, BitmapVectorEvent);
wxDECLARE_EVENT(kEventBitmapChanged, BitmapVectorEvent);
wxDECLARE_EVENT(kEventBitmapNotChanged, BitmapVectorEvent);

class Rescaler;

std::vector<SBitmap *> BitmapPageToBitmapPtr(BitmapVector *page);

class BitmapCtrl : public wxEvtHandler {
 private:
  std::vector<SBitmap *> vec_bitmap_;
  PositionCtrl *pos_ctrl_ = nullptr;
  Rescaler *rescaler_ = nullptr;
  ScrolledImageWindow *window_ = nullptr;
  BitmapVector *bmp_vec_;

 public:
  BitmapCtrl(ScrolledImageWindow *window, PositionCtrl *position,
             Rescaler *rescaler);

  BitmapVector *GetBitmapVec() { return bmp_vec_; }
  const BitmapVector *GetBitmapVec() const { return bmp_vec_; }

  std::vector<SBitmap *> GetVectorPtr();
  std::vector<const SBitmap *> GetVectorPtr() const;

  void SetBitmapVec(BitmapVector *bmp_vec);

  void AdjustBitmap();

  void RecalcPosition(const std::vector<SBitmap *> &bitmap) const;
  void RecalcPosition();
  wxSize GetSize() const;
  wxSize GetSize(const std::vector<SBitmap *> &bitmap) const;

  void Clear();

 protected:
  ScrolledImageWindow *GetWindow() { return window_; };
  Rescaler *GetRescaler() { return rescaler_; }
  PositionCtrl *GetPosCtrl() { return pos_ctrl_; }

 private:
  void OnWindowSize(wxSizeEvent &event);
  void OnImageLoaded(queue::LoadImageEvent &event);
  void DoChangeBitmapVector(BitmapVectorEvent &event);
};

}  // namespace bitmap

}  // namespace fmr

#endif /* end of include guard: FMR_BITMAP_BITMAP_CTRL */
