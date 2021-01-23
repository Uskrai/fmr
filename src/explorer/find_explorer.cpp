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
#include <fmr/explorer/find_explorer.h>
#include <fmr/handler/abstract_handler.h>
#include <fmr/handler/handler_factory.h>
#include <wx/filename.h>
#include <wx/image.h>
#include <wx/log.h>

#include <memory>

namespace fmr {

namespace explorer {

wxDEFINE_EVENT(kEventStreamFound, FoundEvent);

FoundEvent::FoundEvent(const FoundEvent &event) : wxCommandEvent(event) {
  source_stream_ = event.source_stream_;

  if (event.found_stream_)
    found_stream_ = std::unique_ptr<SStream>(new SStream(*event.found_stream_));
}

void FindThread::SetParameter(std::vector<SStream *> &list_stream) {
  list_stream_ = list_stream;
}

#define TEST_RETURN() \
  if (TestDestroy()) return false

wxThread::ExitCode FindThread::Entry() {
  wxLogMessage("Starting Find thread");
  for (auto &it : list_stream_) {
    if (!TestDestroy()) {
      std::unique_ptr<AbstractOpenableHandler> handler(
          HandlerFactory::NewOpenableHandler(it->GetHandlerPath()));

      auto event = std::make_unique<FoundEvent>(
          FoundEvent(kEventStreamFound, GetEventId()));

      event->SetSourceStream(it);
      event->SetFoundStream(std::make_unique<SStream>(*it));

      wxLogMessage("Starting to search for %s/%s", handler->GetName(),
                   it->GetName());
      if (Find(handler.get(), event.get())) {
        // release pointer ownership because event is used by wxQueueEvent
        event.release();
      }
    }

    Update();
    if (TestDestroy()) break;
  }
  wxLogMessage("Find thread completed");
  Completed();
  return (wxThread::ExitCode)0;
}

void FindThread::StreamFound(FoundEvent *event) {
  if (TestDestroy()) return;
  wxLogMessage("Sending FoundEvent to %p", GetParent());
  QueueEventParent(event);
}

template <typename T>
bool FindThread::TraverseHandler(T *handler, FoundEvent *event) {
  wxLogMessage("Traversing %s", handler->GetName());
  handler->Traverse(false);

  bool is_found = false;
  for (const auto &it : handler->GetChild()) {
    TEST_RETURN();

    event->SetFoundStream(std::make_unique<SStream>(SStream(it)));
    TEST_RETURN();
    if (Find(handler, event)) {
      if (Is(kFindThreadOnlyFirstItem))
        return true;
      else
        is_found = true;
    }
  }
  return is_found;
}

bool FindThread::Find(FoundEvent *event) {
  TEST_RETURN();

  auto search_stream = event->GetFoundStream();

  if (!CheckStream(*search_stream)) {
    wxLogMessage("Can't Read %s/%s", search_stream->GetHandlerPath(),
                 search_stream->GetName());
    return false;
  }

  TEST_RETURN();
  StreamFound(event);
  wxLogMessage("Item found in handler %s/%s\n", search_stream->GetHandlerPath(),
               search_stream->GetName());
  return true;
}

bool FindThread::Find(AbstractOpenableHandler *handler, FoundEvent *event) {
  auto search_stream = event->GetFoundStream();
  std::string path = handler->GetItemPath(*search_stream);

  std::unique_ptr<AbstractOpenableHandler> stream_handler(
      HandlerFactory::NewOpenableHandler(path));

  TEST_RETURN();

  // Get Stream from handler if openable or is check handler
  if (HandlerFactory::IsOpenable(path) || Is(kFindThreadCheckHandler)) {
    handler->GetStream(*search_stream);
  }

  TEST_RETURN();

  // if not openable
  if (!HandlerFactory::IsOpenable(path)) {
    std::unique_ptr<AbstractHandler> non_openable_handler(
        HandlerFactory::NewHandler(path));

    // traverse non-openable handler and if item found and with flags Only first
    // item, return
    if (TraverseHandler(non_openable_handler.get(), event) &&
        Is(kFindThreadOnlyFirstItem)) {
      return true;
    }
  }

  if (Find(event)) return true;

  if (path != stream_handler->GetName()) return false;

  TEST_RETURN();

  if (Is(kFindThreadRecursive))
    return TraverseHandler(stream_handler.get(), event);

  return false;
}

bool FindThread::Find(AbstractHandler *handler, FoundEvent *event) {
  auto search_stream = event->GetFoundStream();

  handler->GetStream(*search_stream);

  return Find(event);
};

};  // namespace explorer

};  // namespace fmr
