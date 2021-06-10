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

#ifndef INCLUDE_FMR_FS_FILESYSTEM_H_
#define INCLUDE_FMR_FS_FILESYSTEM_H_

#include <fmr/fs/iterator.h>
#include <fmr/fs/provider.h>
#include <fmr/nowide/fs.h>

namespace fmr {

namespace fs {

class FileSystem : public InputContainer<nwd::fs::path> {
  nwd::fs::path path_;

  InputIterator<nwd::fs::path> end_;
  InputIterator<nwd::fs::path> it_;

 public:
  FileSystem(nwd::fs::path path);
  InputIterator<nwd::fs::path> &iterator() override;
  InputIterator<nwd::fs::path> &end() override;
};

class FileSystemProvider : public Provider {
 public:
  UniqueInputContainer Open(const path &path) override;
};

}  // namespace fs

}  // namespace fmr

#endif  // INCLUDE_FMR_FS_FILESYSTEM_H_
