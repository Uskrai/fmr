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

bool FindHandler::ProcessItem(value_type &item) {
  if (IsBeingStopped()) return false;

  FindStatus ret;
  ret = Find(item);

  if (ret == kFindNotFound) {
    item.SetStatus(kFindNotFound);
    SendItem(std::move(item));
  } else if (ret == kFindBeingStopped)
    return false;

  return true;
}

#define TEST_RETURN() \
  if (IsBeingStopped()) return kFindBeingStopped

FindStatus FindHandler::Find(FindItem &item) {
  auto search_stream = item.GetFoundStream();
  std::string handler_path = search_stream.GetHandlerPath();

  if (HandlerFactory::IsOpenable(handler_path)) {
    auto handler = stream_util::MakeParentOpenableHandler(search_stream);

    TEST_RETURN();

    return Find(handler.get(), item);
  } else {
    auto handler = stream_util::MakeParentHandler(search_stream);

    if (handler) {
      return Find(handler.get(), item);
    }
  }
  return kFindNotFound;
}

FindStatus FindHandler::Find(AbstractOpenableHandler *handler, FindItem &item) {
  std::string path = handler->GetItemPath(item.GetFoundStream());

  FindStatus ret_val = kFindNotFound;

  // Get Stream from handler if openable or is check handler
  if (HandlerFactory::IsOpenable(path)) {
    if (CheckAndSendIfFound(handler, item) == kFindItemFound)
      ret_val = kFindItemFound;

    if (ret_val == kFindItemFound && Is(kFindHandlerOnlyFirstItem))
      return ret_val;

    if (!Is(kFindHandlerRecursive)) return ret_val;

    std::unique_ptr<AbstractOpenableHandler> stream_handler(
        HandlerFactory::NewOpenableHandler(path));
    if (path != stream_handler->GetName()) return ret_val;

    auto temp = TraverseHandler(stream_handler.get(), item);

    if (temp < ret_val) ret_val = temp;
    return ret_val;
  } else if (!HandlerFactory::IsOpenable(path)) {
    std::unique_ptr<AbstractHandler> non_openable_handler(
        HandlerFactory::NewHandler(path));

    TEST_RETURN();
    // traverse non-openable handler and if item found and with flags Only first
    // item, return
    if (TraverseHandler(non_openable_handler.get(), item) == kFindItemFound) {
      return kFindItemFound;
    } else if (!Is(kFindHandlerCheckHandler)) {
      return kFindNotFound;
    }
  }

  return ret_val;
}

template <typename T>
FindStatus FindHandler::TraverseHandler(T *handler, FindItem &item) {
  wxLogMessage("Traversing %s", handler->GetName());

  SStream stream;
  std::vector<SStream> vec_stream;
  bool cont = handler->GetFirst(stream, kDirDefault, false);

  while (cont) {
    vec_stream.push_back(std::move(stream));

    cont = handler->GetNextStream(stream, false);

    TEST_RETURN();
  }

  std::sort(vec_stream.begin(), vec_stream.end(), Compare::NaturalSortable);

  FindStatus is_found = kFindNotFound;
  for (const auto &it : vec_stream) {
    TEST_RETURN();

    auto child_item = FindItem(item.GetSourceStream(), it);

    TEST_RETURN();
    if (Find(handler, child_item) == kFindItemFound) {
      if (Is(kFindHandlerOnlyFirstItem))
        // return after once if only find first item
        return kFindItemFound;
      else
        is_found = kFindItemFound;
    }
  }
  return is_found;
}

FindStatus FindHandler::Find(AbstractHandler *handler, FindItem &item) {
  return CheckAndSendIfFound(handler, item);
}

template <typename T>
FindStatus FindHandler::CheckAndSendIfFound(T *handler, FindItem &item) {
  TEST_RETURN();

  auto temp_item = item;
  auto status =
      GetChecker()->Check(*this, *handler, temp_item.GetFoundStream());
  temp_item.SetStatus(status);
  if (status == kFindItemFound) SendItem(std::move(temp_item));

  return status;
}

}  // namespace queue

}  // namespace fmr
