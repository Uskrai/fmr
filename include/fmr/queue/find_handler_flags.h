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

#ifndef FMR_QUEUE_FIND_HANDLER_FLAGS
#define FMR_QUEUE_FIND_HANDLER_FLAGS

#include <fmr/common/bitmask.h>

namespace fmr {

namespace queue {

enum FindHandlerFlags {
  kFindHandlerDefault = 0x00,
  kFindHandlerRecursive = 0x01,
  kFindHandlerCheckHandler = 0x02,
  kFindHandlerOnlyFirstItem = 0x08,
  kFindHandlerDontRecursiveNonOpenable = 0x10,
  kFindHandlerCheckFilenameIfOpenable = 0x20
};
DEFINE_BITMASK_TYPE(FindHandlerFlags)

enum FindStatus {
  kFindItemFound = 0,
  kFindBeingStopped = 1,
  kFindNotFound = 2
};

[[deprecated("Changed to FindStatus")]] typedef FindStatus FindReturn;

}  // namespace queue

}  // namespace fmr
/// flags used by FindHandler

#endif /* end of include guard: FMR_QUEUE_FIND_HANDLER_FLAGS */
