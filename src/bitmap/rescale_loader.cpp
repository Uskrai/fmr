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

#include "fmr/bitmap/rescale_loader.h"

namespace fmr {

namespace bitmap {

RescaleLoader::RescaleLoader(wxEvtHandler *parent, int id)
    : Loader(parent, id), rescale_controller_(this, kRescaleImageThreadID) {
  SetControllerId(GetFindController()->GetThreadId(),
                  GetLoadImageController()->GetEventId(),
                  GetRescaleController()->GetThreadId());

  Bind(kEventThreadCompleted, &RescaleLoader::OnThreadCompleted, this);
}

void RescaleLoader::SetControllerId(int find_id, int load_id, int rescale_id) {
  GetLoadImageController()->Unbind(queue::kEventImageLoaded,
                                   &RescaleLoader::OnImageLoaded, this,
                                   GetLoadImageController()->GetEventId());

  GetRescaleController()->Unbind(queue::kEventImageRescaled,
                                 &RescaleLoader::OnImageRescaled, this,
                                 GetRescaleController()->GetThreadId());

  SetControllerId(find_id, load_id);
  GetRescaleController()->SetThreadId(rescale_id);

  GetLoadImageController()->Bind(queue::kEventImageLoaded,
                                 &RescaleLoader::OnImageLoaded, this,
                                 GetLoadImageController()->GetEventId());

  GetRescaleController()->Bind(queue::kEventImageRescaled,
                               &RescaleLoader::OnImageRescaled, this,
                               GetRescaleController()->GetThreadId());
}

void RescaleLoader::OnThreadCompleted(wxThreadEvent &event) {
  if (event.GetId() == GetLoadImageController()->GetEventId()) {
    GetRescaleController()->DisableOnEmptyQueue(true);
  }
  event.Skip();
}

void RescaleLoader::OnImageLoaded(queue::LoadImageEvent &event) {
  stream_bmp_.insert(std::make_pair(event.GetStream(), event.GetBitmap()));

  wxImage *img = &stream_bmp_[event.GetStream()].GetImage();
  img_stream_.insert(std::make_pair(img, event.GetStream()));

  GetRescaleController()->Push(img);
}

void RescaleLoader::OnImageRescaled(queue::RescaledEvent &event) {
  auto stream_item = img_stream_.find(event.GetImage());
  if (stream_item != img_stream_.end()) {
    auto item = stream_bmp_.find(stream_item->second);
    if (item != stream_bmp_.end()) SendImageToParent(item->first, item->second);
  }
}

}  // namespace bitmap

}  // namespace fmr
