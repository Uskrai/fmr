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

#ifndef FMR_READER_DECORATOR
#define FMR_READER_DECORATOR

#include <memory>

#include "fmr/bitmap/bitmap_page_ctrl.h"
#include "fmr/reader/page_indicator.h"
#include "fmr/window/decorator.h"
#include "fmr/window/scrolled_image.h"

namespace fmr {

namespace reader {

class DecoratorCtrl : public WindowDecoratorList {
  std::unique_ptr<PageIndicator> page_indicator_ = nullptr;
  bitmap::BitmapPageCtrl *bitmap_page_ctrl_;

 public:
  DecoratorCtrl() = delete;
  DecoratorCtrl(ScrolledImageWindow *window,
                bitmap::BitmapPageCtrl *bmp_page_ctrl) {
    Create(window, bmp_page_ctrl);
  };

  PageIndicator *GetPageIndicator() { return page_indicator_.get(); }

  void SetWindow(ScrolledImageWindow *window);
  ScrolledImageWindow *GetWindow();

  void Create(ScrolledImageWindow *window,
              bitmap::BitmapPageCtrl *bmp_page_ctrl);

 private:
  void OnBitmapChanged(bitmap::BitmapVectorEvent &event);
};

}  // namespace reader

}  // namespace fmr

#endif /* end of include guard: FMR_READER_DECORATOR */
