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

#include "fmr/bitmap/loader/rescale.h"

#include "fmr/bitmap/image_checker.h"
#include "fmr/queue/event.h"
#include "fmr/queue/load_image.h"
#include "fmr/queue/rescale.h"
#include "fmr/thread/controller_factory.h"
#include "fmr/thread/rescale_controller.h"

namespace fmr {

namespace bitmap {

namespace loader {

wxDEFINE_EVENT(kOnImageRescaled, RescaleEvent);

Rescale::Rescale(int event_id) : Base(event_id) {
  rescale_data_ = std::make_unique<RescaleQueueData>(this, event_id);
  rescale_data_->CreateReceiver(this, event_id);
  rescale_data_->GetReceiver()->SetEventType(kOnImageRescaled);

  rescale_data_->CreateTask(rescale_data_->GetReceiver());

  rescale_data_->Run();

  // rescale_receiver_ = std::make_unique<RescaleReceiverEvent>(this, event_id);
  // rescale_receiver_->SetEventType(kOnImageRescaled);
  //
  // rescale_controller_ =
  // std::make_unique<thread::RescaleController>(this, event_id);
  // rescale_task_ =
  // rescale_controller_->CreateTask<queue::Rescale>(rescale_receiver_.get());
  //
  // rescale_controller_->Run();

  SetContainer(std::make_unique<RescaleContainer>());

  Bind(kOnImageRescaled, &Rescale::OnImageRescaled, this, event_id);
}

const RescaleContainer *Rescale::GetContainer() const {
  return static_cast<const RescaleContainer *>(Base::GetContainer());
}

RescaleContainer *Rescale::GetContainer() {
  return static_cast<RescaleContainer *>(Base::GetContainer());
}

void Rescale::SetContainer(std::unique_ptr<RescaleContainer> container) {
  Base::SetContainer(std::move(container));
}

void Rescale::SetRescaler(Rescaler *rescaler) {
  rescale_data_->GetTask()->SetRescaler(rescaler);
}

void Rescale::OnImageLoaded(ImageLoadEvent &event) {
  auto &item = event.GetItem();
  PushRescale(item.GetStream(), item.GetImage());
}

void Rescale::PushRescale(const SStream *found_stream, const wxImage &img) {
  auto img_ptr = GetContainer()->AddImage(found_stream, img);
  rescale_data_->GetQueueCtrl()->Push(img_ptr);
}

void Rescale::OnImageRescaled(RescaleEvent &event) {
  auto found_stream =
      GetContainer()->GetFoundStream(event.GetItem().GetImage());
  if (found_stream) {
    auto source_stream = GetContainer()->GetSourceStream(found_stream);

    SendImage(source_stream, found_stream, *event.GetItem().GetImage());
  }
}

void Rescale::Clear() {
  rescale_data_->Clear();
  Base::Clear();
}

Rescale::~Rescale() { Clear(); }

}  // namespace loader

}  // namespace bitmap

}  // namespace fmr
