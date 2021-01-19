/*
 *  Copyright (c) 2020-2021 Uskrai
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

#ifndef FMR_READER_WINDOW
#define FMR_READER_WINDOW

#include <fmr/bitmap/bitmap.h>
#include <fmr/common/config.h>
#include <fmr/common/path.h>
#include <fmr/common/range.h>
#include <fmr/handler/abstract_handler.h>
#include <fmr/reader/controller_reader.h>
#include <fmr/window/scrolledwindow.h>
#include <wx/event.h>

#include <memory>

namespace fmr {

namespace reader {

enum ScrollingType : int { SCROLL_BY_WINDOW, SCROLL_BY_IMAGE, SCROLL_BY_PIXEL };

class LoadThread;

class Window : public ScrolledWindow {
 public:
  Window(wxWindow *parent, wxWindowID id = wxID_ANY,
         const wxPoint &pos = wxDefaultPosition,
         const wxSize &size = wxDefaultSize, long style = wxTAB_TRAVERSAL,
         const wxString &name = wxPanelNameStr);
  Window(wxWindow *parent, wxSize size);
  ~Window();
  bool Destroy();

  bool Open(const std::string &path);

  void ReloadConfig();

  bool ChangeFolder(int step);
  void Next();
  void Prev();

  void Clear();

  std::shared_ptr<AbstractHandler> GetHandler() { return file_handler_; }

 protected:
  void AdjustBitmap();

 private:
  void BindEvent();
  Controller controller_ = Controller(this);
  // pointer to handler
  std::shared_ptr<AbstractHandler> file_handler_ = NULL;
  // vector to bitmaps
  std::shared_ptr<Bitmap> bitmap_ = NULL;
  // pointer to config files
  Config *config_;
  // pointer to scrollbar
  wxSizer *m_sizer;
  bool is_opened_ = false;

  template <typename T>
  T ConfRead(wxString name, T def);

  void CalcScrollStep(ScrollingType type);

  std::shared_ptr<AbstractHandler> NewHandler(const std::string &path);
  std::shared_ptr<Bitmap> NewBitmap();

  void Error(wxSize size);
  void OnDraw(wxDC &dc);

  void OnSize(wxSizeEvent &event);
  void OnMouseMotion(wxMouseEvent &event);
  void OnEdge(wxDirection direction);

  void OnThreadUpdate(wxThreadEvent &event);
  void OnZoomThreadCompleted(wxThreadEvent &event) { AdjustScrollBar(); };
};

};  // namespace reader

};  // namespace fmr
#endif
