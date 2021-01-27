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
      load_controller_(this, kLoadImageThreadID) {
  parent_ = parent;
  GetFindController()->SetChecker(&image_util::CanRead);

  GetFindController()->Bind(thread::kEventStreamFound, &Loader::OnStreamFound,
                            this, kFindImageHandlerThreadID);

  GetLoadImageController()->Bind(thread::kEventImageLoaded,
                                 &Loader::OnImageLoaded, this,
                                 kLoadImageThreadID);

  Bind(EVT_COMMAND_THREAD_COMPLETED, &Loader::OnThreadCompleted, this);
}

void Loader::OnThreadCompleted(wxThreadEvent &event) {
  switch (event.GetId()) {
    case kFindImageHandlerThreadID:
      GetLoadImageController()->DisableOnEmptyQueue(true);
      break;
  }
  event.Skip();
}

bool Loader::Open(const std::string &path) {
  return GetFindController()->Open(path);
}

const SStream *Loader::GetSourceStream(const SStream *found_stream) {
  return GetFindController()->GetSourceStream(found_stream);
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

    send_event->SetStream(event.GetStream());
    send_event->SetImage(event.GetImage());
    wxQueueEvent(GetParent(), send_event.release());
  }
}

bool Loader::Run() {
  Clear();
  bool ret = GetFindController()->Run();
  if (ret) GetFindController()->DisableOnEmptyQueue(true);
  return ret;
}

void Loader::Clear() {
  GetLoadImageController()->Clear();
  GetFindController()->Clear();
}

}  // namespace bitmap

}  // namespace fmr
