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

#include <fmr/file_handler/stream.h>
#ifndef FMR_FILE_HANDLER_READ_STREAM
#define FMR_FILE_HANDLER_READ_STREAM

namespace fmr {

namespace file_handler {

class ReadStream : public Stream {
 public:
  virtual bool IsLoaded() const = 0;
  virtual bool Load() = 0;
  virtual ~ReadStream(){};

  std::unique_ptr<ReadStream> Clone() const {
    return std::unique_ptr<ReadStream>(DoClone());
  }

 protected:
  ReadStream *DoClone() const override = 0;
};

}  // namespace file_handler

}  // namespace fmr

#endif /* end of include guard: FMR_FILE_HANDLER_READ_STREAM */
