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

#include <fmr/bitmap/inc.h>
#include <fmr/bitmap/rescaler.h>
#include <fmr/file_handler/factory.h>
#include <fmr/loader/fwd.h>
#include <fmr/position/inc.h>
#include <fmr/reader/decorator_ctrl.h>
#include <fmr/reader/scroll_controller.h>
#include <fmr/window/scrolled_image.h>

namespace fmr {

namespace reader {

class Settings;
wxDECLARE_EVENT(kEventOpenFile, wxCommandEvent);

enum ControllerId { kLoaderId = wxID_HIGHEST + 1500, kOpenedTimer };

class Controller : public ScrollController {
  std::unique_ptr<loader::Page> loader_;
  std::unique_ptr<position::BoxCtrl> position_ctrl_;
  std::unique_ptr<bitmap::BitmapPageCtrl> bitmap_ctrl_;
  std::unique_ptr<bitmap::Rescaler> rescaler_;
  std::unique_ptr<DecoratorCtrl> decorator_;
  bool is_read_from_right_ = false;
  wxWindow *parent_ = nullptr;

  const file_handler::Factory *handler_factory_ =
      file_handler::Factory::GetGlobal();

  wxTimer opened_timer_{this, kOpenedTimer};
  bool is_opened_ = false;
  int opened_delay_;
  float window_fit_size_ = 1;

 public:
  Controller();
  Controller(wxWindow *parent, wxWindowID id,
             const wxPoint &pos = wxDefaultPosition,
             const wxSize &size = wxDefaultSize, long style = 0,
             const wxString &name = wxPanelNameStr);

  virtual ~Controller();

  bool CreateWindow(wxWindow *parent, wxWindowID id,
                    const wxPoint &pos = wxDefaultPosition,
                    const wxSize &size = wxDefaultSize, long style = 0,
                    const wxString &name = wxPanelNameStr);

  bool Open(const std::string &path);
  void AdjustBitmap();

  void SetSettings(const Settings &setting);

  file_handler::Handler *GetHandler();
  virtual void Clear();

  void SetImagePerPage(size_t size);

  bitmap::BitmapPageCtrl *GetBitmapCtrl() { return bitmap_ctrl_.get(); }
  const bitmap::BitmapPageCtrl *GetBitmapCtrl() const {
    return bitmap_ctrl_.get();
  }

  void GoToPage(size_t idx, wxDirection direction = wxDOWN);
  void ChangePage(wxDirection direction);

  bool ChangeFolder(wxDirection direction);

  wxSize GetFitSize();
  wxWindow *GetParent() { return parent_; }

 private:
  void OnLoadedImage(loader::LoadEvent &event);
  void OnOpenedStreamFound(wxCommandEvent &event);
  void OnWindowScroll(wxScrollWinEvent &event);
  void OnWindowSize(wxSizeEvent &event);
  void OnBitmapChanged(bitmap::BitmapVectorEvent &event);
  void OnBitmapPageNotFound(bitmap::BitmapVectorEvent &event);
  void OnBitmapVectorPushed(bitmap::BitmapVectorEvent &event);

  void OnOpenedTimer(wxTimerEvent &event);
};

}  // namespace reader

}  // namespace fmr

#endif /* end of include guard: FMR_READER_CONTROLLER */
