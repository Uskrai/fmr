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

Loader::Loader(wxEvtHandler *parent, int id)
    : find_controller_(this, kFindImageHandlerThreadID),
      load_controller_(this, kLoadImageThreadID) {
  parent_ = parent;

  SetEventId(id);
  GetFindController()->SetChecker(&image_util::CanRead);

  GetFindController()->SetAutoRun(true);
  GetLoadImageController()->SetAutoRun(true);

  GetFindController()->DisableOnEmptyQueue(true);
  GetLoadImageController()->DisableOnEmptyQueue(true);

  SetControllerId(kFindImageHandlerThreadID, kLoadImageThreadID);
  Bind(kEventThreadCompleted, &Loader::OnThreadCompleted, this);
}

void Loader::OnThreadCompleted(wxThreadEvent &event) {
  if (event.GetId() == find_controller_.GetThreadId()) {
    GetLoadImageController()->DisableOnEmptyQueue(true);
  }
  event.Skip();
}

void Loader::SetControllerId(int find_controller_id,
                             int load_image_controller_id) {
  GetFindController()->Unbind(queue::kEventStreamFound, &Loader::OnStreamFound,
                              this, GetFindController()->GetThreadId());

  GetLoadImageController()->Unbind(queue::kEventImageLoaded,
                                   &Loader::OnImageLoaded, this,
                                   GetLoadImageController()->GetEventId());

  GetFindController()->SetThreadId(find_controller_id);
  GetLoadImageController()->SetEventId(load_image_controller_id);

  GetFindController()->Bind(queue::kEventStreamFound, &Loader::OnStreamFound,
                            this, GetFindController()->GetThreadId());

  GetLoadImageController()->Bind(queue::kEventImageLoaded,
                                 &Loader::OnImageLoaded, this,
                                 GetLoadImageController()->GetEventId());
}

bool Loader::Open(const std::string &path) {
  return GetFindController()->Open(path);
}

const SStream *Loader::GetSourceStream(const SStream *found_stream) {
  return GetFindController()->GetSourceStream(found_stream);
}

void Loader::OnStreamFound(queue::FoundEvent &event) {
  auto item = event.GetSourceStream();

  if (GetFindController()->IsInQueue(item)) {
    GetLoadImageController()->Push(event.GetFoundStream());

    GetFindController()->AddFoundStream(event.GetSourceStream(),
                                        event.GetFoundStreamOwnerShip());
  }
}

void Loader::SendImageToParent(const SStream *stream, const SBitmap &bitmap) {
  auto send_event = std::make_unique<queue::LoadImageEvent>(
      queue::kEventImageLoaded, GetEventId());

  send_event->SetStream(stream);
  send_event->SetBitmap(bitmap);
  wxQueueEvent(GetParent(), send_event.release());
}

void Loader::OnImageLoaded(queue::LoadImageEvent &event) {
  auto source_stream = GetSourceStream(event.GetStream());

  if (source_stream) SendImageToParent(event.GetStream(), event.GetBitmap());
}

bool Loader::Run() {
  bool ret = true;

  if (!GetFindController()->IsRunning()) {
    ret = GetFindController()->Run();
    ret = GetLoadImageController()->Run() && ret;
  }
  return true;
}

void Loader::ClearThread() {
  GetLoadImageController()->ClearThread();
  GetFindController()->ClearThread();
}

void Loader::Clear() {
  ClearThread();
  GetLoadImageController()->Clear();
  GetFindController()->Clear();
}

}  // namespace bitmap

}  // namespace fmr
