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

#include "fmr/queue/find_handler.h"

#include <iostream>
#include <memory>

#include "fmr/compare/natural.h"
#include "fmr/file_handler/factory.h"
#include "fmr/handler/stream_util.h"
#include "fmr/queue/find_handler_flags.h"
#include "wx/filename.h"
#include "wx/image.h"
#include "wx/log.h"

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

FindStatus FindHandler::CheckAndSendIfFound(FindItem &item) {
  TEST_RETURN();

  auto temp_item = item;
  auto status = GetChecker()->Check(*temp_item.GetFoundStream(), this);

  temp_item.SetStatus(status);
  if (status == kFindItemFound) {
    SendItem(std::move(temp_item));
  }

  return status;
}

FindStatus FindHandler::Find(FindItem &item) {
  auto ret_val = CheckAndSendIfFound(item);

  TEST_RETURN();

  switch (ret_val) {
    case kFindItemFound:
      if (Is(kFindHandlerOnlyFirstItem)) {
        return ret_val;
      }
    case kFindNotFound:
      if (Is(kFindHandlerRecursive)) {
        auto handler = factory_->NewHandler(*item.GetFoundStream());
        if (handler && handler->IsOk() && handler->IsExist()) {
          return TraverseHandler(*handler, item);
        }
      }
      break;
    case kFindBeingStopped:
      return kFindBeingStopped;
  }

  return ret_val;
}

FindStatus FindHandler::TraverseHandler(file_handler::Handler &handler,
                                        FindItem &item) {
  auto cont = handler.Read()->GetFirst(false);
  while (cont) {
    cont = handler.Read()->GetNext(false);
    TEST_RETURN();
  }

  handler.Read()->Sort(compare::Natural());

  FindStatus is_found = kFindNotFound;
  for (const auto &it : *handler.Read()) {
    TEST_RETURN();

    auto child_item = FindItem(item.GetSourceStream(), it.Clone());

    TEST_RETURN();
    if (Find(child_item) == kFindItemFound) {
      if (Is(kFindHandlerOnlyFirstItem)) {
        return kFindItemFound;
      } else
        is_found = kFindItemFound;
    }
  }
  return is_found;
}

}  // namespace queue

}  // namespace fmr
