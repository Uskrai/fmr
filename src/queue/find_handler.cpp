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

#include <fmr/common/string.h>
#include <fmr/handler/abstract_handler.h>
#include <fmr/handler/handler_factory.h>
#include <fmr/queue/find_handler.h>
#include <wx/filename.h>
#include <wx/image.h>
#include <wx/log.h>

#include <iostream>
#include <memory>

#include "fmr/handler/stream_util.h"

namespace fmr {

namespace queue {

wxDEFINE_EVENT(kEventStreamFound, FoundEvent);
wxDEFINE_EVENT(kEventStreamNotFound, FoundEvent);

FoundEvent::FoundEvent(const FoundEvent &event) : wxCommandEvent(event) {
  source_stream_ = event.source_stream_;

  if (event.found_stream_)
    found_stream_ = std::make_unique<SStream>(*event.found_stream_);
}

bool FindHandler::ProcessTask(value_type &item) {
  auto event = MakeEvent(kEventStreamFound, GetEventId(), item,
                         std::make_unique<SStream>(*item));

  wxLogMessage("Starting to search for %s/%s",
               event->GetFoundStream()->GetHandlerPath(),
               event->GetFoundStream()->GetName());

  if (IsBeingStopped()) return false;

  FindReturn ret = Find(event.get());

  if (ret == kFindNotFound) {
    event->SetEventType(kEventStreamNotFound);
    SendEventToParent(event.release());
  } else if (ret == kFindBeingStopped)
    return false;

  return true;
}

#define TEST_RETURN() \
  if (IsBeingStopped()) return kFindBeingStopped

FindReturn FindHandler::Find(FoundEvent *event) {
  auto search_stream = event->GetFoundStream();
  std::string handler_path = search_stream->GetHandlerPath();

  if (HandlerFactory::IsOpenable(handler_path)) {
    auto handler = stream_util::MakeParentOpenableHandler(*search_stream);

    TEST_RETURN();

    return Find(handler.get(), event);
  } else {
    auto handler = stream_util::MakeParentHandler(*search_stream);

    if (handler) {
      return Find(handler.get(), event);
    }
  }
  return kFindNotFound;
}

FindReturn FindHandler::Find(AbstractOpenableHandler *handler,
                             FoundEvent *event) {
  auto search_stream = event->GetFoundStream();
  std::string path = handler->GetItemPath(*search_stream);

  FindReturn ret_val = kFindNotFound;

  // Get Stream from handler if openable or is check handler
  if (HandlerFactory::IsOpenable(path)) {
    if (CheckAndSendIfFound(handler, event) == kFindItemFound)
      ret_val = kFindItemFound;

    if (ret_val == kFindItemFound && Is(kFindHandlerOnlyFirstItem))
      return ret_val;

    if (!Is(kFindHandlerRecursive)) return ret_val;

    std::unique_ptr<AbstractOpenableHandler> stream_handler(
        HandlerFactory::NewOpenableHandler(path));
    if (path != stream_handler->GetName()) return ret_val;

    auto temp = TraverseHandler(stream_handler.get(), event);

    if (temp < ret_val) ret_val = temp;
    return ret_val;
  } else if (!HandlerFactory::IsOpenable(path)) {
    std::unique_ptr<AbstractHandler> non_openable_handler(
        HandlerFactory::NewHandler(path));

    TEST_RETURN();
    // traverse non-openable handler and if item found and with flags Only first
    // item, return
    if (TraverseHandler(non_openable_handler.get(), event) == kFindItemFound) {
      return kFindItemFound;
    } else if (!Is(kFindHandlerCheckHandler)) {
      return kFindNotFound;
    }
  }

  return ret_val;
}

void FindHandler::StreamFound(FoundEvent *event) {
  wxLogMessage("Sending FoundEvent to %p", GetParent());
  SendEventToParent(event);
}

template <typename T>
FindReturn FindHandler::TraverseHandler(T *handler, FoundEvent *event) {
  wxLogMessage("Traversing %s", handler->GetName());
  handler->Traverse(false);

  FindReturn is_found = kFindNotFound;
  for (const auto &it : handler->GetChild()) {
    TEST_RETURN();

    auto child_event =
        MakeEvent(kEventStreamFound, GetEventId(), event->GetSourceStream(),
                  std::make_unique<SStream>(it));

    TEST_RETURN();
    if (Find(handler, child_event.get()) == kFindItemFound) {
      if (Is(kFindHandlerOnlyFirstItem))
        // return after once if only find first item
        return kFindItemFound;
      else
        is_found = kFindItemFound;
    }
  }
  return is_found;
}

FindReturn FindHandler::Find(AbstractHandler *handler, FoundEvent *event) {
  return CheckAndSendIfFound(handler, event);
};

void FindHandler::SendFoundEvent(FoundEvent *event) {
  auto send_event =
      std::make_unique<FoundEvent>(kEventStreamFound, event->GetId());
  send_event->SetSourceStream(event->GetSourceStream());
  send_event->SetFoundStream(event->GetFoundStreamOwnerShip());

  SendEventToParent(send_event.release());
}

template <typename T>
FindReturn FindHandler::CheckAndSendIfFound(T *handler, FoundEvent *event) {
  TEST_RETURN();

  auto status = GetChecker()->Check(*this, *handler, *event->GetFoundStream());
  if (status == kFindItemFound) SendFoundEvent(event);

  return status;
}

};  // namespace queue

};  // namespace fmr
