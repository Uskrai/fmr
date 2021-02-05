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

#include "fmr/thread/find_handler_controller.h"

#include "fmr/handler/handler_factory.h"

namespace fmr {
namespace thread {

FindHandlerController::FindHandlerController(wxEvtHandler *parent, int id) {
  parent_ = parent;

  SetThreadId(id);
}

bool FindHandlerController::Open(const std::string &path) {
  auto handler = HandlerFactory::NewHandler(path);

  if (handler) {
    handler->Traverse();

    if (handler->Size() == 0) return false;

    for (const auto &it : handler->GetChild()) {
      Push(&it);
    }
    handler_ = std::move(handler);
    return true;
  }

  return false;
}

void FindHandlerController::SetThreadId(int id) {
  Unbind(queue::kEventStreamFound, &FindHandlerController::OnStreamFound, this,
         thread_id_);
  thread_id_ = id;
  Bind(queue::kEventStreamFound, &FindHandlerController::OnStreamFound, this,
       thread_id_);
}

void FindHandlerController::AddFoundStream(
    const SStream *source, std::unique_ptr<SStream> &&found_stream) {
  found_source_map_.insert(std::make_pair(found_stream.get(), source));

  loaded_stream_.push_back(std::move(found_stream));
}

void FindHandlerController::OnStreamFound(queue::FoundEvent &event) {
  AddFoundStream(event.GetSourceStream(), event.GetFoundStreamOwnerShip());
}

const SStream *FindHandlerController ::GetSourceStream(const SStream *stream) {
  auto item = found_source_map_.find(stream);

  if (item != found_source_map_.end()) {
    return item->second;
  }
  return nullptr;
}

bool FindHandlerController::IsInQueue(const SStream *stream) const {
  for (const auto &it : in_queue_vec_) {
    if (it == stream) return true;
  }
  return false;
}

void FindHandlerController::Clear() {
  DeleteThread(thread_, lock_);
  found_source_map_.clear();
  loaded_stream_.clear();
}

bool FindHandlerController::Run() {
  Clear();

  thread_ =
      new Queue<queue::FindHandler>(this, wxTHREAD_DETACHED, GetThreadId());

  while (!stream_queue_.empty()) {
    auto item = stream_queue_.front();
    thread_->GetQueue().Push(item);
    in_queue_vec_.push_back(item);
    stream_queue_.pop();
  }

  thread_->GetQueue().SetChecker(stream_checker_);
  thread_->GetQueue().SetFlags(thread_flags_);

  return thread_->Run() == wxTHREAD_NO_ERROR;
}

void FindHandlerController::DisableOnEmptyQueue(bool disable) {
  if (thread_) {
    thread_->DisableOnEmptyQueue(disable);
  }
}

void FindHandlerController::DoSetNull(BaseThread *thread) {
  if (thread == thread_) thread_ = nullptr;
}

}  // namespace thread
}  // namespace fmr
