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
  find_controller_.SetChecker(&image_util::CanRead);

  find_controller_.Bind(thread::kEventStreamFound, &Loader::OnStreamFound, this,
                        kFindImageHandlerThreadID);

  load_controller_.Bind(thread::kEventImageLoaded, &Loader::OnImageLoaded, this,
                        kLoadImageThreadID);

  Bind(EVT_COMMAND_THREAD_COMPLETED, &Loader::OnThreadCompleted, this);

  rescale_controller_.Bind(thread::kEventImageRescaled,
                           &Loader::OnImageRescaled, this,
                           kRescaleImageThreadID);
}

void Loader::OnThreadCompleted(wxThreadEvent &event) {
  //
}

bool Loader::Open(const std::string &path) {
  return find_controller_.Open(path);
}

void Loader::OnStreamFound(thread::FoundEvent &event) {
  auto item = event.GetSourceStream();

  if (find_controller_.IsInQueue(item)) {
    load_controller_.Push(event.GetFoundStream());
    event.Skip();
  }
}

void Loader::OnImageLoaded(thread::LoadImageEvent &event) {
  auto source_stream = find_controller_.GetSourceStream(event.GetStream());

  if (source_stream) {
    queue_in_rescale_.push(event.GetImage());
    auto &image = queue_in_rescale_.back();
    map_loaded_to_source_.insert(std::make_pair(&image, event.GetStream()));
    rescale_controller_.Push(&image);
  }
}

void Loader::OnImageRescaled(thread::RescaledEvent &event) {
  auto item = map_loaded_to_source_.find(event.GetImage());

  if (item != map_loaded_to_source_.end()) {
    auto source_stream = find_controller_.GetSourceStream(item->second);

    auto send_event = std::make_unique<thread::LoadImageEvent>(
        thread::kEventImageLoaded, load_controller_.GetThreadId());

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
  return find_controller_.Run();
}

void Loader::Clear() {
  rescale_controller_.Clear();
  load_controller_.Clear();
  find_controller_.Clear();
}

BaseThread *Loader::GetThread(int id) {
  switch (id) {
    case kLoadImageThreadID:
      return load_image_thread_;
  }
  return nullptr;
}

void Loader::DoSetNull(int id) {
  switch (id) {
    case kLoadImageThreadID:
      load_image_thread_ = nullptr;
  }
}

}  // namespace bitmap

}  // namespace fmr
