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
#include <fmr/thread/find_handler.h>
#include <wx/filename.h>
#include <wx/image.h>
#include <wx/log.h>

#include <memory>

namespace fmr {

namespace thread {

wxDEFINE_EVENT(kEventStreamFound, FoundEvent);
wxDEFINE_EVENT(kEventStreamNotFound, FoundEvent);

FoundEvent::FoundEvent(const FoundEvent &event) : wxCommandEvent(event) {
  source_stream_ = event.source_stream_;

  if (event.found_stream_)
    found_stream_ = std::unique_ptr<SStream>(new SStream(*event.found_stream_));
}

bool FindHandler::Push(const SStream *stream) {
  std::pair<const SStream *, SStream> item;
  item.first = stream;

  if (stream) {
    item.second = *stream;
    Push(std::move(item));
    return true;
  }
  return false;
}
#define TEST_RETURN() \
  if (TestDestroy()) return false

wxThread::ExitCode FindHandler::Entry() {
  wxLogMessage("Starting Find thread");

  while (!TestDestroy()) {
    if (!QueueEmpty()) {
      auto &item = Front();

      auto event = MakeEvent(kEventStreamFound, GetEventId(), item.first,
                             std::make_unique<SStream>(item.second));

      wxLogMessage("Starting to search for %s/%s", item.second.GetHandlerPath(),
                   item.second.GetName());

      if (!TestDestroy()) {
        if (!Find(event.get())) {
          event->SetEventType(kEventStreamNotFound);
          QueueEventParent(event.release());
        }
        Update();
        if (TestDestroy()) break;
      }
      Pop();
    }
  };
  wxLogMessage("Find thread completed");
  Completed();
  return (wxThread::ExitCode)0;
}

bool FindHandler::Find(FoundEvent *event) {
  auto search_stream = event->GetFoundStream();
  std::string handler_path = search_stream->GetHandlerPath();

  if (HandlerFactory::IsOpenable(handler_path)) {
    auto handler = HandlerFactory::NewOpenableHandler(handler_path);

    TEST_RETURN();

    return Find(handler.get(), event);
  } else {
    auto handler = HandlerFactory::NewHandler(handler_path);

    if (handler) {
      return Find(handler.get(), event);
    }
  }
  return false;
}

bool FindHandler::Find(AbstractOpenableHandler *handler, FoundEvent *event) {
  auto search_stream = event->GetFoundStream();
  std::string path = handler->GetItemPath(*search_stream);

  std::unique_ptr<AbstractOpenableHandler> stream_handler(
      HandlerFactory::NewOpenableHandler(path));

  TEST_RETURN();

  // Get Stream from handler if openable or is check handler
  if (HandlerFactory::IsOpenable(path) || Is(kFindHandlerCheckHandler)) {
    handler->GetStream(*search_stream);
  }

  TEST_RETURN();

  // if not openable
  if (!HandlerFactory::IsOpenable(path)) {
    std::unique_ptr<AbstractHandler> non_openable_handler(
        HandlerFactory::NewHandler(path));

    TEST_RETURN();
    // traverse non-openable handler and if item found and with flags Only first
    // item, return
    if (TraverseHandler(non_openable_handler.get(), event) &&
        Is(kFindHandlerOnlyFirstItem)) {
      return true;
    } else if (!Is(kFindHandlerCheckHandler)) {
      return false;
    }
  }

  TEST_RETURN();
  if (SendIfFound(event)) return true;

  if (path != stream_handler->GetName()) return false;

  TEST_RETURN();

  if (Is(kFindHandlerRecursive))
    return TraverseHandler(stream_handler.get(), event);

  return false;
}

void FindHandler::StreamFound(FoundEvent *event) {
  if (TestDestroy()) return;
  wxLogMessage("Sending FoundEvent to %p", GetParent());
  QueueEventParent(event);
}

template <typename T>
bool FindHandler::TraverseHandler(T *handler, FoundEvent *event) {
  wxLogMessage("Traversing %s", handler->GetName());
  handler->Traverse(false);

  bool is_found = false;
  for (const auto &it : handler->GetChild()) {
    TEST_RETURN();

    auto child_event =
        MakeEvent(kEventStreamFound, GetEventId(), event->GetSourceStream(),
                  std::make_unique<SStream>(it));

    TEST_RETURN();
    if (Find(handler, child_event.get())) {
      if (Is(kFindHandlerOnlyFirstItem))
        return true;
      else
        is_found = true;
    }
  }
  return is_found;
}

bool FindHandler::Find(AbstractHandler *handler, FoundEvent *event) {
  auto search_stream = event->GetFoundStream();

  TEST_RETURN();
  handler->GetStream(*search_stream);

  TEST_RETURN();
  return SendIfFound(event);
};

bool FindHandler::SendIfFound(FoundEvent *event) {
  TEST_RETURN();

  auto search_stream = event->GetFoundStream();

  TEST_RETURN();

  if (!CheckStream(*search_stream)) {
    wxLogMessage("Can't Read %s/%s", search_stream->GetHandlerPath(),
                 search_stream->GetName());
    return false;
  }

  TEST_RETURN();

  auto send_event =
      std::make_unique<FoundEvent>(kEventStreamFound, GetEventId());

  send_event->SetSourceStream(event->GetSourceStream());
  send_event->SetFoundStream(event->GetFoundStreamOwnerShip());

  TEST_RETURN();

  wxLogMessage("Item found in handler %s/%s\n", search_stream->GetHandlerPath(),
               search_stream->GetName());
  StreamFound(send_event.release());
  return true;
}

};  // namespace thread

};  // namespace fmr
