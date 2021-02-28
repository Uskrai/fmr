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

#include "fmr/bitmap/loader/base.h"

#include "fmr/bitmap/image_checker.h"
#include "fmr/queue/event.h"
#include "fmr/queue/find_handler.h"
#include "fmr/thread/find_handler_controller.h"
#include "fmr/thread/load_image_controller.h"

namespace fmr {

namespace bitmap {

namespace loader {

wxDEFINE_EVENT(kEventOnFindItem, ImageFindEvent);
wxDEFINE_EVENT(kEventOnImageLoaded, ImageLoadEvent);
wxDEFINE_EVENT(kEventImageLoaded, LoadEvent);
wxDEFINE_EVENT(kEventOnFindItemPush, FindEvent);

Base::Base(int event_id) {
  event_id_ = event_id;

  load_receiver_ = std::make_unique<ImageLoadReceiverEvent>(this, event_id);
  load_receiver_->SetEventType(kEventOnImageLoaded);

  load_controller_ =
      std::make_unique<thread::LoadImageController>(this, event_id);
  load_controller_->GetQueue()->SetReceiver(load_receiver_.get());

  find_receiver_ = std::make_unique<ImageFindReceiverEvent>(this, event_id);
  find_receiver_->SetEventType(kEventOnFindItem);

  image_checker_ = std::make_unique<ImageChecker>();

  find_controller_ =
      std::make_unique<thread::FindHandlerController>(this, event_id);
  find_controller_->GetQueue()->SetReceiver(find_receiver_.get());
  find_controller_->GetQueue()->SetChecker(image_checker_.get());

  SetContainer(std::make_unique<Container>());

  Bind(kEventOnFindItem, &Base::OnFindItem, this, GetEventId());
  Bind(kEventOnImageLoaded, &Base::OnImageLoad, this, GetEventId());

  Bind(kEventOnFindItemPush, &Base::OnFindItemPush, this, GetEventId());
}

Container *Base::GetContainer() { return container_.get(); }
const Container *Base::GetContainer() const { return container_.get(); }

void Base::SetContainer(std::unique_ptr<Container> container) {
  container_ = std::move(container);
}

void Base::SetFindFlags(queue::FindHandlerFlags flags) {
  find_controller_->GetQueue()->SetFlags(flags);
}

void Base::SendImage(const SStream *source_stream, const SStream *found_stream,
                     const wxImage &image) {
  LoadEvent send_event(GetEventId(), kEventImageLoaded);
  send_event.SetStream(source_stream, found_stream);
  send_event.SetBitmap(image);

  wxPostEvent(this, send_event);
}

void Base::PushFind(const SStream *stream) {
  GetContainer()->InsertFind(stream);
  find_controller_->Push(stream);
}

void Base::PushLoad(const SStream *source_stream, SStream *stream) {
  GetContainer()->InsertLoad(stream);
  load_controller_->Push(stream);
}

void Base::MakeFrontLoad(const SStream *source_stream, SStream *found_stream) {
  if (!IsInLoadQueue(found_stream)) {
    PushLoad(source_stream, found_stream);
  }
  load_controller_->MakeFront(found_stream);
}

void Base::LoadFoundStream(const SStream *found_stream) {
  if (GetContainer()->IsFound(found_stream)) {
    auto source_stream = GetContainer()->GetSourceStream(found_stream);
    auto item = GetContainer()->GetFoundStream(source_stream);
    auto stream = std::find(item.begin(), item.end(), found_stream);
    if (stream != item.end()) PushLoad(GetSourceStream(found_stream), *stream);
  }
}

void Base::LoadSourceStream(const SStream *source_stream) {
  auto item = GetContainer()->GetFoundStream(source_stream);
  for (auto &it : item) PushLoad(source_stream, it);
}

bool Base::IsFound(const SStream *found_stream) const {
  return GetContainer()->IsFound(found_stream);
}

bool Base::IsSourceFound(const SStream *source_stream) const {
  return GetContainer()->IsSourceFound(source_stream);
}

const SStream *Base::GetSourceStream(const SStream *found_stream) const {
  return GetContainer()->GetSourceStream(found_stream);
}

std::vector<SStream *> Base::GetFoundStream(
    const SStream *source_stream) const {
  return GetContainer()->GetFoundStream(source_stream);
}

bool Base::IsInFindQueue(const SStream *stream) {
  return GetContainer()->IsInFindQueue(stream);
}

bool Base::IsInLoadQueue(const SStream *found_stream) {
  return GetContainer()->IsInLoadQueue(found_stream);
}

void Base::OnImageLoaded(ImageLoadEvent &event) {
  auto &item = event.GetItem();

  auto found_stream = item.GetStream();
  auto source_stream = GetSourceStream(found_stream);
  SendImage(source_stream, found_stream, event.GetItem().GetImage());
}

void Base::OnFindItem(ImageFindEvent &event) {
  auto &item = event.GetItem();
  if (IsInFindQueue(item.GetSourceStream()) &&
      item.GetStatus() != queue::kFindNotFound) {
    auto found_stream = GetContainer()->AddFoundStream(item.GetSourceStream(),
                                                       item.GetFoundStream());

    FindEvent send_event(GetEventId(), kEventOnFindItemPush);
    send_event.SetStream(item.GetSourceStream(), found_stream);

    wxPostEvent(this, send_event);
  }
  GetContainer()->RemoveFind(event.GetItem().GetSourceStream());
}

void Base::OnImageLoad(ImageLoadEvent &event) {
  GetContainer()->RemoveLoad(event.GetItem().GetStream());
  OnImageLoaded(event);
}

void Base::OnFindItemPush(FindEvent &event) {
  if (!IsLazyLoad()) PushLoad(event.GetSourceStream(), event.GetFoundStream());
}

void Base::Clear() {
  load_controller_->Clear();
  find_controller_->Clear();
  GetContainer()->Clear();
}

Base::~Base() { Clear(); }

}  // namespace loader

}  // namespace bitmap

}  // namespace fmr
