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

#include "fmr/bitmap/loader.h"
#include "fmr/queue/event.h"
#include "fmr/thread/find_handler_controller.h"
#include "fmr/thread/load_image_controller.h"
#include "fmr/thread/rescale_controller.h"

namespace fmr {

namespace bitmap {

wxDEFINE_EVENT(kEventRescale, RescaleEvent);

RescaleLoader::RescaleLoader(wxEvtHandler *parent, int id)
    : Loader(parent, id) {
  rescale_controller_ =
      thread::controller_factory::NewRescale(this, kRescaleImageThreadID);

  rescale_receiver_ =
      std::make_unique<RescaleReceiverEvent>(this, GetEventId());
  GetRescaleController()->GetQueue()->SetReceiver(rescale_receiver_.get());

  SetControllerId(GetFindController()->GetEventId(),
                  GetLoadImageController()->GetEventId(),
                  GetRescaleController()->GetEventId());

  Bind(kEventThreadCompleted, &RescaleLoader::OnThreadCompleted, this);
}

void RescaleLoader::SetControllerId(int find_id, int load_id, int rescale_id) {
  Unbind(kEventRescale, &RescaleLoader::OnRescaled, this, GetEventId());
  Unbind(kEventImageLoad, &RescaleLoader::OnItemLoaded, this, GetEventId());

  Loader::SetControllerId(find_id, load_id);

  rescale_receiver_->SetEventId(GetEventId());
  rescale_receiver_->SetEventType(kEventRescale);

  Bind(kEventRescale, &RescaleLoader::OnRescaled, this, GetEventId());
  Bind(kEventImageLoad, &RescaleLoader::OnItemLoaded, this, GetEventId());
}

void RescaleLoader::SetRescaler(bitmap::Rescaler *rescaler) {
  GetRescaleController()->GetQueue()->SetRescaler(rescaler);
}

void RescaleLoader::OnThreadCompleted(wxThreadEvent &event) { event.Skip(); }

void RescaleLoader::OnItemLoaded(ImageLoadEvent &event) {
  stream_bmp_.insert(
      std::make_pair(event.GetItem().GetStream(), event.GetItem().GetImage()));

  wxImage *image = &stream_bmp_[event.GetItem().GetStream()];
  img_stream_.insert(std::make_pair(image, event.GetItem().GetStream()));

  GetRescaleController()->Push(image);
}

void RescaleLoader::OnRescaled(RescaleEvent &event) {
  auto stream_item = img_stream_.find(event.GetItem().GetImage());
  if (stream_item != img_stream_.end()) {
    auto item = stream_bmp_.find(stream_item->second);
    if (item != stream_bmp_.end()) {
      SendImageToParent(item->first, *event.GetItem().GetImage());
    }
  }
}

void RescaleLoader::Clear() {
  Loader::Clear();
  GetRescaleController()->Clear();
  stream_bmp_.clear();
  img_stream_.clear();
}

RescaleLoader::~RescaleLoader() {}

}  // namespace bitmap

}  // namespace fmr
