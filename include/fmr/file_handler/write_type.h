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

#ifndef FMR_FILE_HANDLER_WRITE_TYPE
#define FMR_FILE_HANDLER_WRITE_TYPE

#include <fmr/common/bitmask.h>

namespace fmr {

namespace file_handler {

/*! \enum WriteType
 *
 */
enum WriteType {
  kWriteNone = 0x01,
  kWriteOverwrite = 0x02,
  kWriteIfNotExist = 0x04,
  kWriteDirectory = 0x08,
  kWriteDelete = 0x10
};

DEFINE_BITMASK_TYPE(WriteType)

}  // namespace file_handler

}  // namespace fmr

#endif /* end of include guard: FMR_FILE_HANDLER_WRITE_TYPE */
