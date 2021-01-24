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
#include <fmr/common/vector.h>
#include <fmr/explorer/controller_explorer.h>
#include <wx/log.h>

namespace fmr {

namespace explorer {

Controller::Controller(wxWindow *parent) {
  parent_ = parent;

  Bind(thread::kEventStreamFound, &Controller::OnFound, this);
  Bind(EVT_BITMAP_LOADED, &Controller::OnLoaded, this, kLoadThreadID);
  Bind(EVT_COMMAND_THREAD_UPDATE, &Controller::OnUpdate, this, kLoadThreadID);
  Bind(EVT_COMMAND_THREAD_COMPLETED, &Controller::OnFindCompleted, this,
       kFindThreadID);
}

Controller::~Controller() {
  DeleteThread(kLoadThreadID, g_sLock);
  DeleteThread(kFindThreadID, g_sLock);
}

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
  Delete(kLoadThreadID, g_sLock);
  Delete(kFindThreadID, g_sLock);
}

void Controller::SetParameter(std::vector<StreamBitmap> &list_stream) {
  list_stream_ = list_stream;
}

void Controller::OnFound(thread::FoundEvent &event) {
  auto item = map_item_.find(event.GetSourceStream());
  if (item != map_item_.end()) {
    StreamBitmap stream_bitmap;
    stream_bitmap.bitmap = map_item_[event.GetSourceStream()];

    auto stream = event.GetFoundStreamOwnerShip();

    stream_bitmap.stream = stream.release();

    if (load_thread_) {
      load_thread_->Push(stream_bitmap);
    }
  }
}

void Controller::OnLoaded(StreamBitmapEvent &event) {}

void Controller::OnUpdate(wxThreadEvent &event) { Update(event.GetId()); }

void Controller::OnFindCompleted(wxThreadEvent &event) {
  if (load_thread_) load_thread_->DeleteOnEmptyQueue();
}

void Controller::SetThumbSize(const wxSize &size) { thumb_size_ = size; }

void Controller::Load() {
  DeleteThread(kFindThreadID, g_sLock);
  DeleteThread(kLoadThreadID, g_sLock);

  map_item_.clear();
  std::vector<SStream *> vec_stream;
  for (auto &it : list_stream_) {
    vec_stream.push_back(it.stream);

    map_item_.insert(std::make_pair(it.stream, it.bitmap));
  }

  find_thread_ =
      new thread::FindHandler(this, wxTHREAD_DETACHED, kFindThreadID);
  find_thread_->DisableEventOnDestroy();
  find_thread_->SetParameter(vec_stream);
  find_thread_->SetChecker(&image_util::CanRead);

  find_thread_->SetFlags(thread::kFindHandlerRecursive |
                         thread::kFindHandlerOnlyFirstItem);
  map_item_.clear();
  for (auto &it : list_stream_) {
    find_thread_->Push(it.stream);

    map_item_.insert(std::make_pair(it.stream, it.bitmap));
  }

  load_thread_ = new LoadThread(this, wxTHREAD_DETACHED, kLoadThreadID);
  load_thread_->SetSize(thumb_size_);

  load_thread_->Run();
  find_thread_->Run();
}

};  // namespace explorer

};  // namespace fmr
