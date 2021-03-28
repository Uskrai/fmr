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

#include <fmr/bitmap/image_util.h>
#include <wx/log.h>

#include "fmr/nowide/string.h"

namespace fmr {

namespace image_util {

// bool CanRead(const SStream &stream) {
// return stream.IsOk() && wxImage::CanRead(*stream.GetStream());
// }
//
// bool CanRead(const std::string &name) {
// return wxImage::CanRead(String::Widen<wxString>(name));
// }

bool Load(wxImage &image, wxInputStream &stream) {
  if (!stream.IsOk() && stream.GetSize() == 0) return false;

  if (!wxImage::CanRead(stream)) return false;

  wxLogNull nuller;
  return image.LoadFile(stream);
}

// bool Load(wxImage &image, const SStream &stream) {
// std::shared_ptr<wxInputStream> input_stream;
// if (!stream.IsOk()) return false;
//
// input_stream = stream.GetStream();
//
// if (input_stream) return Load(image, *input_stream);
//
// return false;
// }
//
// bool Load(wxImage &image, const AbstractHandler &handler, size_t index) {
// if (!handler.IsExist(index)) return false;
//
// return Load(image, handler.Item(index));
// }

bool Rescale(wxImage &image, float scale, wxImageResizeQuality image_quality) {
  wxSize original_size = image.GetSize();
  if (scale <= 0 && image.IsOk()) return false;

  wxSize scaled_size = image.GetSize().Scale(scale, scale);

  if (original_size != scaled_size)
    image.Rescale(scaled_size.GetWidth(), scaled_size.GetHeight(),
                  image_quality);

  return original_size != scaled_size;
}

bool Rescale(SBitmap &bmp, wxImage &img, float scale,
             wxImageResizeQuality image_quality) {
  if (!Rescale(img, scale, image_quality)) return false;

  if (img.GetSize() == bmp.GetSize()) return false;

  bmp.SetImage(img);
  bmp.SetScale(scale, scale);

  return true;
}

float scaling(float original, float dest) { return (dest / original); }

bool Rescale(wxImage &image, wxSize max_size,
             wxImageResizeQuality image_quality) {
  wxSize image_size = image.GetSize();

  float scale_height = scaling(image_size.GetHeight(), max_size.GetHeight());

  float scale_width = scaling(image_size.GetWidth(), max_size.GetWidth());

  float scale = 1;
  if (scale_height > scale_width && max_size.GetWidth() > -1)
    scale = scale_width;
  else if (max_size.GetHeight() > -1)
    scale = scale_height;

  return Rescale(image, scale, image_quality);
}

std::vector<bitmap::BitmapDraw> CreateBitmapDraw(const wxImage &image) {
  wxPoint pos(0, 0);
  std::vector<bitmap::BitmapDraw> bmp_vec_;

  if (!image.IsOk()) return bmp_vec_;

  while (pos.x < image.GetWidth() && pos.y < image.GetHeight()) {
    bitmap::BitmapDraw bmp(image, pos);

    auto area = bmp.GetSlicedArea();

    pos.x += area.GetWidth();

    bmp_vec_.push_back(std::move(bmp));

    if (pos.x >= image.GetWidth()) {
      pos.y += area.GetHeight();
      pos.x = 0;
    }
  }

  return bmp_vec_;
}

}  // namespace image_util

}  // namespace fmr
