/*
 *  Copyright (c) 2020-2021 Uskrai
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
#include <fmr/bitmap/rescaler.h>
#include <fmr/common/vector.h>
#include <fmr/explorer/controller_explorer.h>
#include <wx/log.h>

#include <functional>

namespace fmr {

namespace explorer {

Controller::Controller(wxWindow *parent) {
  parent_ = parent;
  rescaler_ = bitmap::Rescaler(bitmap::kRescaleFitAll);

  Bind(thread::kEventStreamFound, &Controller::OnFound, this);
  Bind(thread::kEventImageLoaded, &Controller::OnImageLoaded, this,
       kLoadThreadID);
  Bind(EVT_COMMAND_THREAD_UPDATE, &Controller::OnUpdate, this, kLoadThreadID);
  Bind(EVT_COMMAND_THREAD_COMPLETED, &Controller::OnFindCompleted, this,
       kFindThreadID);
}

Controller::~Controller() { Clear(); }

void Controller::DoSetNull(int id) {
  switch (id) {
    case kLoadThreadID:
      load_thread_ = NULL;
      break;
    case kFindThreadID:
      find_thread_ = NULL;
      break;
  };
}

wxThread *Controller::GetThread(int id) {
  switch (id) {
    case kLoadThreadID:
      return load_thread_;
    case kFindThreadID:
      return find_thread_;
  }
  return NULL;
}

void Controller::Clear() {
  map_find_.clear();
  map_loading_.clear();
  DeleteThread(kFindThreadID, g_sLock);
  DeleteThread(kLoadThreadID, g_sLock);
  list_found_stream_.clear();
}

void Controller::SetParameter(std::vector<StreamBitmap> &list_stream) {
  list_stream_ = list_stream;
}

void Controller::OnFound(thread::FoundEvent &event) {
  auto item = map_find_.find(event.GetSourceStream());
  if (item != map_find_.end()) {
    auto stream = event.GetFoundStreamOwnerShip();
    map_loading_.insert(
        std::make_pair(stream.get(), map_find_[event.GetSourceStream()]));

    if (load_thread_) {
      load_thread_->Push(stream.get());
    }
    list_found_stream_.push_back(std::move(stream));
  }
}

void Controller::OnImageLoaded(thread::LoadImageEvent &event) {
  auto stream = event.GetStream();
  auto iterator = map_loading_.find(stream);

  if (iterator != map_loading_.end()) {
    rescaler_.DoRescale(event.GetImage());
    iterator->second->SetBitmap(event.GetImage());
    Update(event.GetId());
  }
}

void Controller::OnUpdate(wxThreadEvent &event) {}

void Controller::OnFindCompleted(wxThreadEvent &event) {
  if (load_thread_) load_thread_->DeleteOnEmptyQueue();
}

void Controller::SetThumbSize(const wxSize &size) {
  thumb_size_ = size;
  rescaler_.SetMaximumSize(size);
}

void Controller::Load() {
  Clear();

  find_thread_ =
      new thread::FindHandler(this, wxTHREAD_DETACHED, kFindThreadID);
  find_thread_->DisableEventOnDestroy();
  find_thread_->SetChecker(&image_util::CanRead);

  find_thread_->SetFlags(thread::kFindHandlerRecursive |
                         thread::kFindHandlerOnlyFirstItem);

  for (auto &it : list_stream_) {
    find_thread_->Push(it.stream);
    map_find_.insert(std::make_pair(it.stream, it.bitmap));
  }

  bitmap::Rescaler rescaler(bitmap::kRescaleFitAll);
  rescaler.SetMaximumSize(thumb_size_);
  load_thread_ = new thread::LoadImage(this, wxTHREAD_DETACHED, kLoadThreadID);
  load_thread_->SetSize(thumb_size_);
  load_thread_->SetRescaller(rescaler);

  load_thread_->Run();
  find_thread_->Run();
}

};  // namespace explorer

};  // namespace fmr
