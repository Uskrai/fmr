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

#ifndef FMR_FILE_HANDLER_LOCAL_READER
#define FMR_FILE_HANDLER_LOCAL_READER

#include <fmr/file_handler/input.h>
#include <fmr/file_handler/local/read_stream.h>

namespace fmr {

namespace file_handler {

namespace local {

using InputBase = fmr::file_handler::Input;
class Input : public InputBase {
 public:
  virtual ReadStream *GetFirst(bool get_buffer) override = 0;
  virtual ReadStream *GetNext(bool get_buffer) override = 0;

  virtual void GetChild(std::vector<StreamBase *> &vec) override {
    InputGetChildHelper<Input, ReadStream, StreamBase>(this, vec);
  }

  virtual void GetChild(std::vector<const StreamBase *> &vec) const override {
    InputGetChildHelper<const Input, const ReadStream, const StreamBase>(this,
                                                                         vec);
  }

  virtual void GetChild(std::vector<ReadStream *> &vec) = 0;
  virtual void GetChild(std::vector<const ReadStream *> &vec) const = 0;

  std::vector<ReadStream *> GetChild() {
    std::vector<ReadStream *> vec;
    GetChild(vec);
    return vec;
  }
  std::vector<const ReadStream *> GetChild() const {
    std::vector<const ReadStream *> vec;
    GetChild(vec);
    return vec;
  }
};

//

}  // namespace local

}  // namespace file_handler

}  // namespace fmr

#endif /* end of include guard: FMR_FILE_HANDLER_LOCAL_READER */
