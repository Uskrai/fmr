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

#ifndef FMR_FILE_HANDLER_WRITER
#define FMR_FILE_HANDLER_WRITER

#include <fmr/file_handler/stream.h>
#include <fmr/file_handler/write_type.h>

namespace fmr {

namespace file_handler {

class Output {
 public:
  Output() {}
  virtual ~Output() {}

  virtual bool CreateDirectory(const std::string &name) = 0;
  virtual bool CreateFile(Stream *stream, WriteType type) = 0;

  virtual bool DeleteDirectory(const std::string &name) = 0;
  virtual bool DeleteFile(const std::string &name) = 0;

  virtual bool CommitWrite() = 0;
};

}  // namespace file_handler

}  // namespace fmr

#endif /* end of include guard: FMR_FILE_HANDLER_WRITER */
