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

#ifndef FMR_READER_CONTROLLER
#define FMR_READER_CONTROLLER

#include "fmr/bitmap/bitmap_page_ctrl.h"
#include "fmr/bitmap/page_loader.h"
#include "fmr/bitmap/position_ctrl.h"
#include "fmr/reader/decorator.h"
#include "fmr/reader/scroll_controller.h"
#include "fmr/window/scrolled_image.h"

namespace fmr {

namespace reader {

wxDECLARE_EVENT(kEventOpenFile, wxCommandEvent);

enum ControllerId {
  kLoaderId = wxID_HIGHEST + 1500,
};

class Controller : public ScrollController {
  std::unique_ptr<bitmap::PageLoader> loader_;
  std::unique_ptr<bitmap::PositionCtrl> position_ctrl_;
  std::unique_ptr<bitmap::BitmapPageCtrl> bitmap_ctrl_;
  std::unique_ptr<bitmap::Rescaler> rescaler_;
  std::unique_ptr<DecoratorList> decorator_;
  bool is_read_from_right_ = false;
  wxWindow *parent_ = nullptr;

 public:
  Controller();
  Controller(wxWindow *parent, wxWindowID id,
             const wxPoint &pos = wxDefaultPosition,
             const wxSize &size = wxDefaultSize, long style = 0,
             const wxString &name = wxPanelNameStr);

  virtual ~Controller() { Clear(); }

  bool CreateWindow(wxWindow *parent, wxWindowID id,
                    const wxPoint &pos = wxDefaultPosition,
                    const wxSize &size = wxDefaultSize, long style = 0,
                    const wxString &name = wxPanelNameStr);

  void SetWindow(ScrolledImageWindow *window);

  bool Open(const std::string &path);
  void AdjustBitmap();

  AbstractHandler *GetHandler() { return loader_->GetHandler(); }
  virtual void Clear();

  void SetImagePerPage(size_t size) { loader_->SetImagePerPage(size); }
  void SetPositionFlags(bitmap::PositionFlags flags);
  void SetScaleFlags(bitmap::RescalerFlags flags);

  bitmap::BitmapPageCtrl *GetBitmapCtrl() { return bitmap_ctrl_.get(); }
  const bitmap::BitmapPageCtrl *GetBitmapCtrl() const {
    return bitmap_ctrl_.get();
  }

  bool GoToPage(size_t idx, wxDirection direction = wxDOWN);
  bool Change(wxDirection direction);
  bool ChangePage(wxDirection direction);
  bool ChangeFolder(wxDirection direction);

  wxWindow *GetParent() { return parent_; }

 private:
  void OnLoadedImage(queue::LoadImageEvent &event);
  void OnOpenedStreamFound(wxCommandEvent &event);
  void OnWindowScroll(wxScrollWinEvent &event);
};

}  // namespace reader

}  // namespace fmr

#endif /* end of include guard: FMR_READER_CONTROLLER */
