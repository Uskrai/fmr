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
#include "fmr/queue/event.h"

namespace fmr {

namespace thread {

FindHandlerController::FindHandlerController(wxEvtHandler *parent, int id)
    : QueueThreadCtrl(parent, id) {
  SetEventId(id);
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

void FindHandlerController::AddFoundStream(
    const SStream *source, std::unique_ptr<SStream> &&found_stream) {
  found_source_map_.insert(std::make_pair(found_stream.get(), source));

  loaded_stream_.push_back(std::move(found_stream));
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
  QueueThreadCtrl::Clear();
  found_source_map_.clear();
  loaded_stream_.clear();
  in_queue_vec_.clear();
}

FindHandlerController::~FindHandlerController() { Clear(); }

}  // namespace thread

}  // namespace fmr
