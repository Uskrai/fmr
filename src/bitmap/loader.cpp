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

#include <fmr/bitmap/image_util.h>
#include <fmr/bitmap/loader.h>

namespace fmr {

namespace bitmap {

Loader::Loader(wxEvtHandler *parent)
    : ThreadController(),
      find_controller_(this, kFindImageHandlerThreadID),
      load_controller_(this, kLoadImageThreadID),
      rescale_controller_(this, kRescaleImageThreadID) {
  parent_ = parent;
  GetFindController()->SetChecker(&image_util::CanRead);

  GetFindController()->Bind(thread::kEventStreamFound, &Loader::OnStreamFound,
                            this, kFindImageHandlerThreadID);

  GetLoadImageController()->Bind(thread::kEventImageLoaded,
                                 &Loader::OnImageLoaded, this,
                                 kLoadImageThreadID);

  GetRescaleController()->Bind(thread::kEventImageRescaled,
                               &Loader::OnImageRescaled, this,
                               kRescaleImageThreadID);

  Bind(EVT_COMMAND_THREAD_COMPLETED, &Loader::OnThreadCompleted, this);
}

void Loader::OnThreadCompleted(wxThreadEvent &event) {
  switch (event.GetId()) {
    case kFindImageHandlerThreadID:
      GetLoadImageController()->DisableOnEmptyQueue(true);
      break;
    case kLoadImageThreadID:
      GetRescaleController()->DisableOnEmptyQueue(true);
  }
  event.Skip();
}

bool Loader::Open(const std::string &path) {
  return GetFindController()->Open(path);
}

void Loader::OnStreamFound(thread::FoundEvent &event) {
  auto item = event.GetSourceStream();

  if (GetFindController()->IsInQueue(item)) {
    GetLoadImageController()->Push(event.GetFoundStream());

    GetFindController()->AddFoundStream(event.GetSourceStream(),
                                        event.GetFoundStreamOwnerShip());
  }
}

void Loader::OnImageLoaded(thread::LoadImageEvent &event) {
  auto source_stream = find_controller_.GetSourceStream(event.GetStream());

  if (source_stream) {
    auto send_event = std::make_unique<thread::LoadImageEvent>(
        thread::kEventImageLoaded, GetLoadImageController()->GetThreadId());

    queue_in_rescale_.push(event.GetImage());
    auto &image = queue_in_rescale_.back();
    map_loaded_to_source_.insert(std::make_pair(&image, event.GetStream()));
    GetRescaleController()->Push(&image);
  }
}

void Loader::OnImageRescaled(thread::RescaledEvent &event) {
  auto item = map_loaded_to_source_.find(event.GetImage());

  if (item != map_loaded_to_source_.end()) {
    auto source_stream = GetFindController()->GetSourceStream(item->second);

    auto send_event = std::make_unique<thread::LoadImageEvent>(
        thread::kEventImageLoaded, GetLoadImageController()->GetThreadId());

    send_event->SetStream(source_stream);
    send_event->SetImage(*event.GetImage());
    wxQueueEvent(GetParent(), send_event.release());

    auto &img = queue_in_rescale_.front();
    if (&img == event.GetImage()) {
      queue_in_rescale_.pop();
    }
  }
}

bool Loader::Run() {
  Clear();
  bool ret = GetFindController()->Run();
  if (ret) GetFindController()->DisableOnEmptyQueue(true);
  return ret;
}

void Loader::Clear() {
  GetRescaleController()->Clear();
  GetLoadImageController()->Clear();
  GetFindController()->Clear();
}

}  // namespace bitmap

}  // namespace fmr
