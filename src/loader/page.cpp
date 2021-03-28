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

#include "fmr/loader/page.h"

#include "fmr/bitmap/bitmap_page_ctrl.h"
#include "fmr/bitmap/bitmap_vector_event.h"
#include "fmr/common/vector.h"
#include "fmr/compare/natural.h"

namespace fmr {

namespace loader {

wxDEFINE_EVENT(kEventOpenedStreamFound, wxCommandEvent);

Page::Page(int event_id, bitmap::BitmapPageCtrl *ctrl) : Loader(event_id) {
  bmp_ctrl_ = ctrl;
  bmp_ctrl_->Bind(bitmap::kEventBitmapChanged, &Page::OnBitmapChanged, this);
}

bool Page::Open(const std::string &path) {
  auto handler = GetHandlerFactory()->NewHandler(path);

  if (!handler) return false;

  if (!handler->IsOk()) {
    if (handler->GetParent()) handler->Open(handler->GetParent()->GetPath());
    if (!handler->IsOk()) return false;
  }

  if (!handler->Read()) return false;

  file_handler::Input &input = *handler->Read();

  input.Traverse(false);
  input.Sort(compare::Natural());

  // return false if handler empty
  if (input.IsEmpty()) return false;

  // store index to check page location later
  opened_index_ = input.Index(path);

  if (opened_index_ > input.Size()) opened_index_ = 0;

  opened_stream_ = input.At(opened_index_);

  if (opened_index_ < GetItemPerPage()) {
    SendOpenedStreamInBack();
  }

  for (const auto &it : input) {
    PushFind(&it);
  }

  handler_ = std::move(handler);
  return true;
}

void Page::SendOpenedStreamInBack() {
  auto idx = stream_page_.size();

  if (idx == 0) idx = 1;

  wxCommandEvent send_event(kEventOpenedStreamFound, GetEventId());
  wxPostEvent(this, send_event);
}

void Page::OnFindItemPush(FindEvent &event) {
  auto found_stream = event.GetFoundStream();

  if (stream_page_.empty() ||
      !(stream_page_.back().size() < GetItemPerPage())) {
    stream_page_.emplace_back();
    bmp_ctrl_->EnlargePage(stream_page_.size());
  }

  stream_page_.back().push_back(found_stream);
  bmp_ctrl_->EnlargeBitmapPage(stream_page_.size() - 1,
                               stream_page_.back().size());

  if (event.GetSourceStream() == opened_stream_) SendOpenedStreamInBack();

  AdjustLoad();
}

bool Page::ShouldLoad(size_t page_pos, size_t img_pos) {
  auto &vec = bmp_ctrl_->GetAllPage();
  bool is_page_exist =
      Vector::IsExist(vec, page_pos) && Vector::IsExist(stream_page_, page_pos);
  if (is_page_exist) {
    bool is_img_exist = Vector::IsExist(vec[page_pos]->GetBitmap(), img_pos) &&
                        Vector::IsExist(stream_page_[page_pos], img_pos);

    if (is_img_exist) {
      bool is_img_not_ok = !vec[page_pos]->GetBitmap()[img_pos].IsOk();

      return is_img_not_ok;
    }
  }
  return false;
}

void Page::AdjustLoad() {
  ClearUnused();
  PreloadImage();
}

void Page::LoadImageIfNeeded(size_t page_pos, size_t img_pos, bool make_front) {
  if (ShouldLoad(page_pos, img_pos)) {
    auto found_stream = stream_page_[page_pos][img_pos];
    if (make_front) {
      if (!MakeFrontLoad(found_stream)) PushFrontLoad(found_stream);
    } else if (!IsInLoadQueue(found_stream))
      PushLoad(found_stream);
  }
}

void Page::LoadPageIfNeeded(size_t page_pos, bool make_front) {
  auto &vec_all = bmp_ctrl_->GetAllPage();
  if (Vector::IsExist(vec_all, page_pos)) {
    auto page_img_size = vec_all[page_pos]->GetBitmap().size();
    if (make_front) {
      size_t idx = page_img_size - 1;
      while (Vector::IsExist(vec_all[page_pos]->GetBitmap(), idx)) {
        LoadImageIfNeeded(page_pos, idx, make_front);
        --idx;
      }
    } else
      for (size_t idx = 0; idx < page_img_size; ++idx)
        LoadImageIfNeeded(page_pos, idx, make_front);
  }
}

void Page::PreloadImage() {
  size_t page_pos = bmp_ctrl_->GetPagePos();
  size_t prev = page_pos - 1, next = page_pos + 1;

  LoadPageIfNeeded(page_pos, true);

  auto load_page = [this](size_t pos, const size_t limit, const int step,
                          bool make_front) {
    size_t idx = 0;
    while (idx < limit && Vector::IsExist(stream_page_, pos)) {
      LoadPageIfNeeded(pos, make_front);
      pos += step;
      ++idx;
    }
  };

  load_page(prev, preload_prev_, -1, false);
  load_page(next, preload_next_, 1, false);
}

void Page::ClearPage(size_t page_pos) {
  auto &vec = bmp_ctrl_->GetAllPage();
  if (Vector::IsExist(vec, page_pos)) {
    vec[page_pos]->ClearImage();
  }
}

void Page::ClearUnused() {
  size_t page_pos = bmp_ctrl_->GetPagePos();
  size_t prev = page_pos - preload_prev_ - 1;
  size_t next = page_pos + preload_next_ + 1;
  auto &vec = bmp_ctrl_->GetAllPage();

  auto clear_page = [&, this](size_t pos, int step) {
    while (Vector::IsExist(vec, pos)) {
      ClearPage(pos);
      pos += step;
    }
  };

  clear_page(prev, -1);
  clear_page(next, 1);
}

void Page::OnBitmapChanged(bitmap::BitmapVectorEvent &event) {
  AdjustLoad();
  event.Skip();
}

file_handler::Handler *Page::GetHandler() { return handler_.get(); }
const file_handler::Handler *Page::GetHandler() const { return handler_.get(); }

size_t Page::GetStreamPage(const ReadStream *stream) const {
  size_t idx = 0;
  for (const auto &it : stream_page_) {
    if (std::find(it.begin(), it.end(), stream) != it.end()) return idx;
    ++idx;
  }
  return -1;
}

size_t Page::GetStreamPosInPage(const ReadStream *stream) const {
  size_t idx;
  for (const auto &vec : stream_page_) {
    idx = 0;
    for (const auto &it : vec) {
      if (it == stream) return idx;
      ++idx;
    }
  }

  return -1;
}

void Page::Clear() {
  stream_page_.clear();

  Loader::Clear();

  stream_page_.clear();
}

Page::~Page() { Clear(); }

}  // namespace loader

}  // namespace fmr
