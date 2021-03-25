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

#ifndef FMR_FILE_HANDLER_BASE
#define FMR_FILE_HANDLER_BASE

#include <fmr/common/compare.h>
#include <fmr/file_handler/input.h>
#include <fmr/file_handler/output.h>
#include <fmr/file_handler/stream.h>

#include <string>
#include <vector>

namespace fmr {

namespace file_handler {

class Handler {
 public:
  Handler() {}
  virtual ~Handler(){};

  virtual bool Open(const std::string &name) = 0;
  virtual bool Open(const ReadStream &stream) = 0;
  virtual std::string GetPath() const = 0;

  virtual std::string GetInternalName(const std::string &name) const = 0;
  virtual std::string GetExternalName(const ReadStream &stream) const = 0;

  virtual Input *Read() = 0;
  virtual const Input *Read() const = 0;

  virtual Output *Write() = 0;
  virtual const Output *Write() const = 0;

  virtual Handler *GetParent() = 0;

  virtual bool IsExist() const = 0;

  virtual bool IsHandleable(const Stream &stream) const = 0;
  virtual bool IsHandleable(const std::string &name) const = 0;
  virtual bool IsOk() const = 0;

  std::unique_ptr<Handler> CreateNew() const {
    return std::unique_ptr<Handler>(DoCreateNew());
  }

 protected:
  virtual Handler *DoCreateNew() const = 0;
};

}  // namespace file_handler

}  // namespace fmr

#endif /* end of include guard: FMR_FILE_HANDLER_BASE */
