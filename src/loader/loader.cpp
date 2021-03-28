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

#include "fmr/loader/loader.h"

#include "fmr/bitmap/image_checker.h"
#include "fmr/queue/event.h"
#include "fmr/queue/find_handler.h"
#include "fmr/queue/load_image.h"
#include "fmr/thread/queue_ctrl.h"

namespace fmr {

namespace loader {

wxDEFINE_EVENT(kEventOnFindItem, ImageFindEvent);
wxDEFINE_EVENT(kEventOnImageLoaded, ImageLoadEvent);
wxDEFINE_EVENT(kEventImageLoaded, LoadEvent);
wxDEFINE_EVENT(kEventOnFindItemPush, FindEvent);

Loader::Loader(int event_id) {
  event_id_ = event_id;

  load_data_ = std::make_unique<LoadQueueData>(this, event_id);
  find_data_ = std::make_unique<FindQueueData>(this, event_id);

  load_data_->CreateReceiver(this, event_id);
  load_data_->GetReceiver()->SetEventType(kEventOnImageLoaded);
  load_data_->CreateTask(load_data_->GetReceiver());

  find_data_->CreateReceiver(this, event_id);
  find_data_->GetReceiver()->SetEventType(kEventOnFindItem);
  find_data_->CreateTask(GetHandlerFactory(), find_data_->GetReceiver());

  image_checker_ = std::make_unique<bitmap::ImageChecker>();
  find_data_->GetTask()->SetChecker(image_checker_.get());

  find_data_->Run();
  load_data_->Run();

  SetContainer(std::make_unique<Container>());

  Bind(kEventOnFindItem, &Loader::OnFindItem, this, GetEventId());
  Bind(kEventOnImageLoaded, &Loader::OnImageLoad, this, GetEventId());

  Bind(kEventOnFindItemPush, &Loader::OnFindItemPush, this, GetEventId());
}

Container *Loader::GetContainer() { return container_.get(); }
const Container *Loader::GetContainer() const { return container_.get(); }

void Loader::SetContainer(std::unique_ptr<Container> container) {
  container_ = std::move(container);
}

void Loader::SetFindFlags(queue::FindHandlerFlags flags) {
  find_data_->GetTask()->SetFlags(flags);
}

void Loader::SendImage(const ReadStream *source_stream,
                       const ReadStream *found_stream, const wxImage &image) {
  LoadEvent send_event(GetEventId(), kEventImageLoaded);
  send_event.SetStream(source_stream, found_stream);
  send_event.SetBitmap(image);

  wxPostEvent(this, send_event);
}

void Loader::PushFind(const ReadStream *stream) {
  GetContainer()->InsertFind(stream);
  GetFindController()->Push(stream);
}

void Loader::PushFrontFind(const ReadStream *stream) {
  GetContainer()->InsertFind(stream);
  GetFindController()->PushFront(stream);
}

bool Loader::MakeFrontFind(const ReadStream *stream) {
  return GetFindController()->MakeFront(stream);
}

void Loader::PushLoad(ReadStream *stream) {
  GetContainer()->InsertLoad(stream);
  GetLoadController()->Push(stream);
}

void Loader::PushFrontLoad(ReadStream *found_stream) {
  GetContainer()->InsertLoad(found_stream);
  GetLoadController()->PushFront(found_stream);
}

bool Loader::MakeFrontLoad(ReadStream *found_stream) {
  return GetLoadController()->MakeFront(found_stream);
}

void Loader::LoadFoundStream(const ReadStream *found_stream) {
  if (GetContainer()->IsFound(found_stream)) {
    auto source_stream = GetContainer()->GetSourceStream(found_stream);
    auto item = GetContainer()->GetFoundStream(source_stream);
    auto stream = std::find(item.begin(), item.end(), found_stream);
    if (stream != item.end()) PushLoad(*stream);
  }
}

void Loader::LoadSourceStream(const ReadStream *source_stream) {
  auto item = GetContainer()->GetFoundStream(source_stream);
  for (auto &it : item) PushLoad(it);
}

bool Loader::IsFound(const ReadStream *found_stream) const {
  return GetContainer()->IsFound(found_stream);
}

bool Loader::IsSourceFound(const ReadStream *source_stream) const {
  return GetContainer()->IsSourceFound(source_stream);
}

const ReadStream *Loader::GetSourceStream(
    const ReadStream *found_stream) const {
  return GetContainer()->GetSourceStream(found_stream);
}

std::vector<ReadStream *> Loader::GetFoundStream(
    const ReadStream *source_stream) const {
  return GetContainer()->GetFoundStream(source_stream);
}

bool Loader::IsInFindQueue(const ReadStream *stream) {
  return GetContainer()->IsInFindQueue(stream);
}

bool Loader::IsInLoadQueue(const ReadStream *found_stream) {
  return GetContainer()->IsInLoadQueue(found_stream);
}

void Loader::OnImageLoaded(ImageLoadEvent &event) {
  auto &item = event.GetItem();

  auto found_stream = item.GetStream();
  auto source_stream = GetSourceStream(found_stream);
  SendImage(source_stream, found_stream, event.GetItem().GetImage());
}

void Loader::OnFindItem(ImageFindEvent &event) {
  auto &item = event.GetItem();
  if (IsInFindQueue(item.GetSourceStream()) &&
      item.GetStatus() != queue::kFindNotFound) {
    auto found_stream = GetContainer()->AddFoundStream(
        item.GetSourceStream(), std::move(item.GetFoundStream()));

    FindEvent send_event(GetEventId(), kEventOnFindItemPush);
    send_event.SetStream(item.GetSourceStream(), found_stream);

    wxPostEvent(this, send_event);
  }
  GetContainer()->RemoveFind(event.GetItem().GetSourceStream());
}

void Loader::OnImageLoad(ImageLoadEvent &event) {
  GetContainer()->RemoveLoad(event.GetItem().GetStream());
  OnImageLoaded(event);
}

const Loader::FindQueueCtrl *Loader::GetFindController() const {
  return find_data_->GetQueueCtrl();
}

Loader::FindQueueCtrl *Loader::GetFindController() {
  return find_data_->GetQueueCtrl();
}

const Loader::LoadQueueCtrl *Loader::GetLoadController() const {
  return load_data_->GetQueueCtrl();
}
Loader::LoadQueueCtrl *Loader::GetLoadController() {
  return load_data_->GetQueueCtrl();
}

void Loader::OnFindItemPush(FindEvent &event) {
  if (!IsLazyLoad() && IsFound(event.GetFoundStream()))
    PushLoad(event.GetFoundStream());
}

void Loader::Clear() {
  load_data_->Clear();
  find_data_->Clear();
  GetContainer()->Clear();
}

Loader::~Loader() { Clear(); }

}  // namespace loader

}  // namespace fmr
