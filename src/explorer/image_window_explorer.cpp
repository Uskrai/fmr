/*
 *  Copyright (c) 2020 Uskrai
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

#include <fmr/bitmap/bmp.h>
#include <fmr/common/path.h>
#include <fmr/explorer/image_window_explorer.h>
#include <wx/dc.h>
#include <wx/dcclient.h>

#include "fmr/common/dimension.h"

namespace fmr {

namespace explorer {

ImageWindow::ImageWindow(wxWindow *parent, wxWindowID id, const wxPoint &pos,
                         const wxSize &size, long style, const wxString &name) {
  Create(parent, id, pos, size, style, name);
}

bool ImageWindow::Create(wxWindow *parent, wxWindowID id, const wxPoint &pos,
                         const wxSize &size, long style, const wxString &name) {
  BindEvent();
  bool ret = wxWindow::Create(parent, id, pos, size, style, name);
  window_text_ = new wxStaticText(
      this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize,
      wxST_NO_AUTORESIZE | wxALIGN_CENTER_HORIZONTAL);

  sizer_ = new wxBoxSizer(wxVERTICAL);
  sizer_->AddSpacer(GetBestBitmapSize(GetSize()).GetHeight());
  sizer_->Add(window_text_, wxSizerFlags(1).Align(wxBOTTOM).Expand());

  SetSizer(sizer_);
  Layout();
  return ret;
}

void ImageWindow::BindEvent() {
  Bind(wxEVT_PAINT, &ImageWindow::OnPaint, this);
  Bind(wxEVT_SIZE, &ImageWindow::OnSize, this);
}

void ImageWindow::SetBitmap(const SBitmap &bmp) {
  bitmap_ = bmp;
  refresh_scheduled_ = true;
}

void ImageWindow::SetStream(SStream *stream) {
  stream_ = stream;
  wxString string;
  if (stream) String::FromUTF8(stream->GetName(), string);

  if (window_text_) {
    window_text_->SetLabel(string);
    // TODO: this should have separate method
    window_text_->SetForegroundColour(GetForegroundColour());
  }
}

const SStream *ImageWindow::GetStream() const { return stream_; }

SStream *ImageWindow::GetStream() { return stream_; }

const SBitmap &ImageWindow::GetBitmap() const { return bitmap_; }

SBitmap &ImageWindow::GetBitmap() { return bitmap_; }

wxSize ImageWindow::GetBestBitmapSize(const wxSize &size) {
  wxSize best_size = size;
  best_size.Scale(1, 0.9);
  return best_size;
}

void ImageWindow::PrepareRect(const wxRect &rect) {
  bitmap_rect_ = rect;
  text_rect_ = rect;

  bitmap_rect_.SetSize(GetBestBitmapSize(rect.GetSize()));

  wxSize txt_size = text_rect_.GetSize();
  txt_size.Scale(1, 0.2);
  text_rect_.SetSize(txt_size);
  text_rect_.SetTop(bitmap_rect_.GetBottom() + 5);

  if (bitmap_.IsOk()) {
    wxRect bmp_rect = wxRect(bitmap_.GetPosition(), bitmap_.GetSize());

    bmp_rect = bmp_rect.CenterIn(bitmap_rect_);
    bitmap_position_ = bmp_rect.GetPosition();

    bitmap_size_ = bitmap_.GetSize();

    bitmap_.SetPosition(bitmap_position_);
  }

  this_rect_ = rect;
}

void ImageWindow::OnPaint(wxPaintEvent &event) {
  wxPaintDC dc(this);
  wxRect rect(wxPoint(0, 0), GetSize());

  if (bitmap_size_ != bitmap_.GetSize()) refresh_scheduled_ = true;

  if (this_rect_ != rect || refresh_scheduled_) {
    PrepareRect(rect);
    refresh_scheduled_ = false;
  }

  if (bitmap_.IsOk()) bitmap_.Draw(dc, wxPoint(0, 0), GetSize());
  // if (bitmap_.IsOk()) dc.DrawBitmap(bitmap_.GetBitmap(), bitmap_position_);
}

void ImageWindow::OnSize(wxSizeEvent &event) { refresh_scheduled_ = true; }

};  // namespace explorer

};  // namespace fmr
