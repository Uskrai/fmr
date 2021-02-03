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

#ifndef FMR_WINDOW_SCROLLED_IMAGE_WINDOW
#define FMR_WINDOW_SCROLLED_IMAGE_WINDOW

#include <fmr/window/decorator.h>
#include <fmr/window/scrolledwindow.h>

#include <vector>

#include "fmr/bitmap/bitmap_page.h"

namespace fmr {

wxDECLARE_EVENT(kEventPageChanged, wxCommandEvent);

class ScrolledImageWindow : public ScrolledWindow {
 private:
  bitmap::BitmapPage *page_ = nullptr;
  WindowDecoratorList *decorator_ = nullptr;

 public:
  ScrolledImageWindow() : ScrolledWindow() {}

  /**
   * @brief: Constructor
   * @see: ScrolledWindow::wxWindow
   */
  ScrolledImageWindow(wxWindow *parent, wxWindowID id,
                      const wxPoint &pos = wxDefaultPosition,
                      const wxSize &size = wxDefaultSize, long style = 0,
                      const wxString &name = wxPanelNameStr) {
    Create(parent, id, pos, size, style, name);
  }

  virtual ~ScrolledImageWindow() { ClearBitmap(); };

  /**
   * @brief: Create Window
   * @see: ScrolledWindow::Create
   */
  bool Create(wxWindow *parent, wxWindowID id,
              const wxPoint &pos = wxDefaultPosition,
              const wxSize &size = wxDefaultSize, long style = 0,
              const wxString &name = wxPanelNameStr);

  /**
   * @brief: Set Bitmap to draw in this window
   */
  void SetBitmapPage(bitmap::BitmapPage *bitmap);
  bitmap::BitmapPage *GetPage() { return page_; }
  const bitmap::BitmapPage *GetPage() const { return page_; }

  [[deprecated("Changed to GetPage()")]] bitmap::BitmapPage *GetBitmapPage() {
    return page_;
  }

  void SetDecorator(WindowDecoratorList *decorator) { decorator_ = decorator; }

  void ClearBitmap() { SetBitmapPage(nullptr); }

  /**
   * @brief: Draw Bitmap
   * use this if you override OnDraw
   * @param: dc device-context to draw bitmap to
   */
  void DrawBitmap(wxDC &dc);

  /**
   * overriden method
   */
  virtual void OnDraw(wxDC &dc) override;
};

}  // namespace fmr

#endif /* end of include guard: FMR_WINDOW_SCROLLED_IMAGE_WINDOW */
