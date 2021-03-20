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

#ifndef FMR_FILE_HANDLER_LOCAL_STREAM
#define FMR_FILE_HANDLER_LOCAL_STREAM

#include <fmr/file_handler/read_stream.h>

namespace fmr {

namespace file_handler {

namespace local {

using StreamBase = fmr::file_handler::ReadStream;
class ReadStream : public StreamBase {
 public:
  virtual std::string GetFullPath() const = 0;

 protected:
  virtual ReadStream *DoClone() const override = 0;
};

//
}  // namespace local

}  // namespace file_handler

}  // namespace fmr

#endif /* end of include guard: FMR_FILE_HANDLER_LOCAL_STREAM */
