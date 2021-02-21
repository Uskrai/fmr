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

#include "fmr/bitmap/image_checker.h"
#include "fmr/handler/abstract_handler.h"
#include "fmr/queue/event.h"
#include "fmr/queue/find_handler.h"
#include "fmr/thread/find_handler_controller.h"
#include "fmr/thread/load_image_controller.h"

namespace fmr {

namespace bitmap {

wxDEFINE_EVENT(kEventImageFind, ImageFindEvent);
wxDEFINE_EVENT(kEventImageLoad, ImageLoadEvent);
wxDEFINE_EVENT(kEventImageLoaded, ImageLoadEvent);

Loader::Loader(wxEvtHandler* parent, int id) {
  parent_ = parent;

  find_controller_ = thread::controller_factory::NewFindHandler(
      this, kFindImageHandlerThreadID);

  load_controller_ =
      thread::controller_factory::NewLoadImage(this, kLoadImageThreadID);

  checker_ = std::make_unique<ImageChecker>();

  find_receiver_ = std::make_unique<ImageFindReceiverEvent>(this, GetEventId());

  load_receiver_ = std::make_unique<ImageLoadReceiverEvent>(this, GetEventId());

  SetEventId(id);
  GetFindController()->GetQueue()->SetChecker(checker_.get());
  GetFindController()->GetQueue()->SetReceiver(find_receiver_.get());

  GetFindController()->SetAutoRun(true);
  GetLoadImageController()->SetAutoRun(true);
  GetLoadImageController()->GetQueue()->SetReceiver(load_receiver_.get());

  SetControllerId(kFindImageHandlerThreadID, kLoadImageThreadID);
  Bind(kEventThreadCompleted, &Loader::OnThreadCompleted, this);
}

void Loader::OnThreadCompleted(wxThreadEvent& event) { event.Skip(); }

void Loader::SetControllerId(int find_controller_id,
                             int load_image_controller_id) {
  Unbind(kEventImageFind, &Loader::OnFindItemFound, this, GetEventId());
  Unbind(kEventImageLoad, &Loader::OnItemLoaded, this, GetEventId());

  find_receiver_->SetEventId(GetEventId());
  find_receiver_->SetEventType(kEventImageFind);

  load_receiver_->SetEventId(GetEventId());
  load_receiver_->SetEventType(kEventImageLoad);

  GetFindController()->SetEventId(find_controller_id);
  GetLoadImageController()->SetEventId(load_image_controller_id);

  Bind(kEventImageFind, &Loader::OnFindItemFound, this, GetEventId());
  Bind(kEventImageLoad, &Loader::OnItemLoaded, this, GetEventId());
}

bool Loader::Open(const std::string& path) {
  return GetFindController()->Open(path);
}

void Loader::PushFind(const SStream* stream) {
  GetFindController()->Push(stream);
}

thread::FindHandlerController* Loader::GetFindController() {
  return find_controller_.get();
}

thread::LoadImageController* Loader::GetLoadImageController() {
  return load_controller_.get();
}

const SStream* Loader::GetSourceStream(const SStream* found_stream) {
  return GetFindController()->GetSourceStream(found_stream);
}

void Loader::SetFindFlags(queue::FindHandlerFlags flags) {
  GetFindController()->GetQueue()->SetFlags(flags);
}

void Loader::OnFindItemFound(ImageFindEvent& event) {
  if (event.GetItem().GetStatus() == queue::kFindNotFound) return;

  if (GetFindController()->IsInQueue(event.GetItem().GetSourceStream())) {
    auto found_stream =
        std::make_unique<SStream>(std::move(event.GetItem().GetFoundStream()));

    GetLoadImageController()->Push(found_stream.get());

    GetFindController()->AddFoundStream(event.GetItem().GetSourceStream(),
                                        std::move(found_stream));
  }
}

void Loader::SendImageToParent(const SStream* stream, const wxImage& image) {
  queue::LoadItem item;
  item.SetImage(image);
  item.SetStream(stream);

  auto send_event = std::make_unique<ImageLoadEvent>(
      GetEventId(), kEventImageLoaded, std::move(item));

  wxQueueEvent(GetParent(), send_event.release());
}

void Loader::OnItemLoaded(ImageLoadEvent& event) {
  auto source_stream = GetSourceStream(event.GetItem().GetStream());
  if (source_stream)
    SendImageToParent(event.GetItem().GetStream(), event.GetItem().GetImage());
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
  GetLoadImageController()->Clear();
  GetFindController()->Clear();
}

Loader::~Loader() { Clear(); }

}  // namespace bitmap

}  // namespace fmr
