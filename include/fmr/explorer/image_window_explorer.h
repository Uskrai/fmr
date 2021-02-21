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

#ifndef FMR_EXPLORER_IMAGE_WINDOW
#define FMR_EXPLORER_IMAGE_WINDOW

#include <fmr/explorer/common.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/window.h>

#include <memory>
#include <vector>

namespace fmr {

namespace explorer {

class ImageWindow : public wxWindow {
 protected:
  wxRect bitmap_rect_, text_rect_, this_rect_;
  wxPoint bitmap_position_;
  wxSize bitmap_size_;
  std::vector<StringDraw> vec_string_draw_;
  std::string string_name_;
  SStream *stream_ = nullptr;
  SBitmap bitmap_;

  bool refresh_scheduled_ = false;

  wxStaticText *window_text_ = nullptr;
  wxSizer *sizer_ = nullptr;

 public:
  ImageWindow(){};
  ImageWindow(wxWindow *parent, wxWindowID id = wxID_ANY,
              const wxPoint &pos = wxDefaultPosition,
              const wxSize &size = wxDefaultSize, long style = 0,
              const wxString &name = wxPanelNameStr);

  bool Create(wxWindow *parent, wxWindowID id = wxID_ANY,
              const wxPoint &pos = wxDefaultPosition,
              const wxSize &size = wxDefaultSize, long style = 0,
              const wxString &name = wxPanelNameStr);

  void SetBitmap(const SBitmap &bmp);
  void SetStream(SStream *stream);

  const SStream *GetStream() const;
  SStream *GetStream();
  const SBitmap &GetBitmap() const;
  SBitmap &GetBitmap();

  static wxSize GetBestBitmapSize(const wxSize &size);

  void Draw(wxDC &dc);

  void PrepareRect(const wxRect &rect);

 private:
  void BindEvent();

  void OnPaint(wxPaintEvent &event);
  void OnSize(wxSizeEvent &event);
};

}  // namespace explorer

}  // namespace fmr

#endif
