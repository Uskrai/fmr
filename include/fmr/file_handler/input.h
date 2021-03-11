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

#ifndef FMR_FILE_HANDLER_READER
#define FMR_FILE_HANDLER_READER

#include <fmr/common/bitmask.h>
#include <fmr/file_handler/stream.h>

#include <vector>

namespace fmr {

namespace file_handler {

enum InputOption {
  kInputNone = 0x01,
  kInputSkipPermissionDenied = 0x02,
  kInputFollowDirectorySymlink = 0x04,
  kInputSkipDirectory = 0x08,
};

DEFINE_BITMASK_TYPE(InputOption);

class Input {
 public:
  Input(){};
  virtual ~Input() {}
  virtual std::string GetPath() const = 0;

  virtual bool IsOpened() const = 0;

  virtual void Traverse(bool load_buffer) {
    auto stream = GetFirst(load_buffer);
    while (stream) stream = GetNext(load_buffer);
  }

  virtual Stream *GetFirst(bool load_buffer) = 0;
  virtual Stream *GetNext(bool load_buffer) = 0;

  virtual bool IsEmpty() const = 0;
  virtual size_t Size() const = 0;

  virtual void GetChild(std::vector<Stream *> &vec) = 0;
  virtual void GetChild(std::vector<const Stream *> &vec) const = 0;
};

template <typename InputClass, typename StreamType, typename StreamBaseType>
void InputGetChildHelper(InputClass *ptr, std::vector<StreamBaseType *> &vec) {
  std::vector<StreamBaseType *> take_vec;
  take_vec.reserve(ptr->Size());
  ptr->GetChild(take_vec);

  vec.reserve(ptr->Size() + take_vec.size());
  for (auto &it : take_vec) vec.push_back(it);
}

}  // namespace file_handler

}  // namespace fmr

#endif /* end of include guard: FMR_FILE_HANDLER_READER */
