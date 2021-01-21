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

#include <fmr/explorer/find_explorer.h>
#include <fmr/handler/abstract_handler.h>
#include <fmr/handler/handler_factory.h>
#include <wx/filename.h>
#include <wx/image.h>
#include <wx/log.h>

#include <memory>

namespace fmr {

namespace explorer {

wxDEFINE_EVENT(EVT_STREAM_FOUND, StreamBitmapEvent);

void FindThread::SetParameter(std::vector<StreamBitmap> &list_stream) {
  list_stream_ = list_stream;
}

#define TEST_RETURN() \
  if (TestDestroy()) return false

wxThread::ExitCode FindThread::Entry() {
  wxLogMessage("Starting Find thread");
  for (auto &it : list_stream_) {
    if (!TestDestroy()) {
      std::unique_ptr<AbstractOpenableHandler> handler(
          HandlerFactory::NewOpenableHandler(it.stream->GetHandlerPath()));

      wxLogMessage("Starting to search for %s/%s", handler->GetName(),
                   it.stream->GetName());
      Find(handler.get(), it);
    }

    Update();
    if (TestDestroy()) break;
  }
  wxLogMessage("Find thread completed");
  Completed();
  return (wxThread::ExitCode)0;
}

void FindThread::StreamFound(StreamBitmap &item) {
  if (TestDestroy()) return;

  std::unique_ptr<StreamBitmapEvent> event(
      new StreamBitmapEvent(EVT_STREAM_FOUND, m_id));

  if (TestDestroy()) return;

  event->SetStreamBitmap(item);

  QueueEventParent(event.release());
}

template <typename T>
bool FindThread::TraverseHandler(T *handler, StreamBitmap &item) {
  wxLogMessage("Traversing %s", handler->GetName());
  handler->Traverse(false);

  for (const auto &it : handler->GetChild()) {
    TEST_RETURN();
    item.stream = std::shared_ptr<SStream>(new SStream(it));
    TEST_RETURN();
    if (Find(handler, item)) return true;
  }
  return false;
}

bool FindThread::Find(StreamBitmap &item) {
  TEST_RETURN();
  if (!wxImage::CanRead(*item.stream->GetStream())) {
    wxLogMessage("Can't Read %s/%s", item.stream->GetHandlerPath(),
                 item.stream->GetName());
    return false;
  }

  TEST_RETURN();
  StreamFound(item);
  wxLogMessage("Item found in handler %s/%s\n", item.stream->GetHandlerPath(),
               item.stream->GetName());
  return true;
}

bool FindThread::Find(AbstractOpenableHandler *handler, StreamBitmap &item) {
  std::string path = handler->GetItemPath(*item.stream);

  if (HandlerFactory::IsOpenable(path)) {
    std::unique_ptr<AbstractOpenableHandler> stream_handler(
        HandlerFactory::NewOpenableHandler(path));

    TEST_RETURN();

    handler->GetStream(*item.stream);

    TEST_RETURN();

    if (Find(item)) return true;

    if (path != stream_handler->GetName()) return false;

    TEST_RETURN();

    return TraverseHandler(stream_handler.get(), item);
  } else {
    std::unique_ptr<AbstractHandler> stream_handler(
        HandlerFactory::NewHandler(path));

    TEST_RETURN();

    return TraverseHandler(stream_handler.get(), item);
  }
  return false;
}

bool FindThread::Find(AbstractHandler *handler, StreamBitmap &item) {
  handler->GetStream(*item.stream);

  return Find(item);
};

};  // namespace explorer

};  // namespace fmr
