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

#include "fmr/bitmap/page_loader.h"

namespace fmr {

namespace bitmap {

wxDEFINE_EVENT(kEventOpenedStreamFound, wxCommandEvent);

PageLoader::PageLoader(wxEvtHandler *parent, BitmapPageCtrl *bmp_ctrl, int id)
    : Loader(parent, id) {
  SetControllerId(GetFindController()->GetThreadId(),
                  GetLoadImageController()->GetThreadId());
  bmp_ctrl_ = bmp_ctrl;
  Bind(EVT_COMMAND_THREAD_COMPLETED, &PageLoader::OnCompleted, this);
}

bool PageLoader::Open(const std::string &path) {
  auto handler = HandlerFactory::NewHandler(path);

  if (handler) {
    handler->Traverse();

    // return false if handler empty
    if (handler->Size() == 0) return false;

    // store index to check page location later
    opened_index_ = handler->Index(path);

    if (!handler->IsExist(opened_index_)) opened_index_ = 0;

    opened_stream_ = &handler->Item(opened_index_);

    if (opened_index_ < GetImagePerPage()) {
      SendOpenedPageInBack();
    }

    for (const auto &it : handler->GetChild()) {
      PushFind(&it);
    }

    handler_ = std::move(handler);
    return true;
  }

  return false;
}

void PageLoader::SetControllerId(int find_controller_id,
                                 int load_image_controller_id) {
  GetFindController()->Unbind(queue::kEventStreamFound,
                              &PageLoader::OnStreamFound, this,
                              GetFindController()->GetThreadId());

  Loader::SetControllerId(find_controller_id, load_image_controller_id);

  GetFindController()->Bind(queue::kEventStreamFound,
                            &PageLoader::OnStreamFound, this,
                            find_controller_id);
}

size_t PageLoader::GetStreamPage(const SStream *stream) {
  size_t idx = 0;
  for (const auto &it : per_page_stream_) {
    for (const auto &jt : it)
      if (jt == stream) return idx;
    ++idx;
  }
  return -1;
}

size_t PageLoader::GetStreamPosInPage(const SStream *stream) {
  size_t idx;
  for (const auto &it : per_page_stream_) {
    idx = 0;
    for (const auto &jt : it) {
      if (jt == stream) return idx;
      ++idx;
    }
  }
  return -1;
}

size_t PageLoader::SourceStreamToIndex(const SStream *stream) {
  size_t idx = 0;
  for (const auto &it : handler_->GetChild()) {
    if (&it == stream) return idx;
    ++idx;
  }
  return -1;
}

const SStream *PageLoader::GetFirstFoundStream(const SStream *source_stream) {
  for (const auto &it : per_page_stream_) {
    for (const auto &found_stream : it) {
      auto stream = GetFindController()->GetSourceStream(found_stream);
      if (stream == source_stream) return found_stream;
    }
  }
  return nullptr;
}

bool PageLoader::IsFoundStreamInBack(const SStream *found_stream) {
  for (const auto &it : per_page_stream_.back()) {
    if (found_stream == it) return true;
  }
  return false;
}

void PageLoader::OnStreamFound(queue::FoundEvent &event) {
  if (per_page_stream_.empty() ||
      per_page_stream_.back().size() >= GetImagePerPage()) {
    per_page_stream_.push_back(std::vector<SStream *>());
  }

  per_page_stream_.back().push_back(event.GetFoundStream());
  size_t event_source_stream_idx = SourceStreamToIndex(event.GetSourceStream());

  GetBitmapCtrl()->EnlargeBitmapPage(per_page_stream_.size() - 1,
                                     per_page_stream_.back().size());

  if (opened_index_ < GetImagePerPage()) {
    GetLoadImageController()->Push(event.GetFoundStream());
  } else {
    if (event_source_stream_idx >= opened_index_) {
      if (event_source_stream_idx == opened_index_) {
        SendOpenedPageInBack();
      }
      if (IsFoundStreamInBack(GetFirstFoundStream(opened_stream_)) ||
          event_source_stream_idx == opened_index_) {
        while (IsFoundStreamInBack(stream_before_opened_.top())) {
          GetLoadImageController()->Push(stream_before_opened_.top());
          stream_before_opened_.pop();
        }
        GetLoadImageController()->Push(event.GetFoundStream());
      } else {
        GetLoadImageController()->Push(event.GetFoundStream());
        PushStreamStack();
      }
    } else {
      stream_before_opened_.push(event.GetFoundStream());
    }
  }

  GetFindController()->AddFoundStream(event.GetSourceStream(),
                                      event.GetFoundStreamOwnerShip());
}

void PageLoader::SendOpenedPageInBack() {
  size_t idx = per_page_stream_.size();

  if (idx != 0) idx -= 1;

  opened_page_ = idx;

  auto event =
      std::make_unique<wxCommandEvent>(kEventOpenedStreamFound, GetEventId());

  event->SetInt(idx);

  wxQueueEvent(GetParent(), event.release());
}

void PageLoader::PushStreamStack() {
  while (!stream_before_opened_.empty()) {
    GetLoadImageController()->Push(stream_before_opened_.top());
    stream_before_opened_.pop();
  }
}

void PageLoader::OnCompleted(wxThreadEvent &event) {
  if (event.GetId() == GetFindController()->GetThreadId()) {
    PushStreamStack();
  }
  event.Skip();
}

size_t PageLoader::CountLoadedImage() {
  size_t ret = 0;
  for (const auto &it : per_page_stream_) {
    ret += it.size();
  }
  return ret;
}

void PageLoader::Clear() {
  Loader::Clear();
  opened_index_ = -1;
  opened_page_ = -1;
  per_page_stream_.clear();
  opened_stream_ = nullptr;
}

}  // namespace bitmap

}  // namespace fmr
