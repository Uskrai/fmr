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

#include "fmr/loader/rescale_container.h"

namespace fmr {

namespace loader {

const ReadStream *RescaleContainer::GetFoundStream(const wxImage *bmp) const {
  auto item = img_to_stream_.find(bmp);
  if (item != img_to_stream_.end()) return item->second;
  return nullptr;
}

wxImage *RescaleContainer::AddImage(const ReadStream *found_stream,
                                    const wxImage &img) {
  stream_to_img_.insert(std::make_pair(found_stream, img));
  auto img_ptr = GetImage(found_stream);
  img_to_stream_.insert(std::make_pair(img_ptr, found_stream));
  return img_ptr;
}

const wxImage *RescaleContainer::GetImage(
    const ReadStream *found_stream) const {
  auto item = stream_to_img_.find(found_stream);
  if (item != stream_to_img_.end()) return &item->second;
  return nullptr;
}

wxImage *RescaleContainer::GetImage(const ReadStream *found_stream) {
  auto item = stream_to_img_.find(found_stream);
  if (item != stream_to_img_.end()) return &item->second;
  return nullptr;
}

void RescaleContainer::Clear() {
  stream_to_img_.clear();
  img_to_stream_.clear();

  Container::Clear();
}

}  // namespace loader

}  // namespace fmr
