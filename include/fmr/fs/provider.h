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

#ifndef INCLUDE_FMR_FS_PROVIDER_H_
#define INCLUDE_FMR_FS_PROVIDER_H_

#include <fmr/find/find.h>
#include <fmr/fs/iterator.h>
#include <fmr/nowide/fs.h>

namespace fmr {

namespace fs {

class Find;

/**
 * This is an abstract class used to Open folder, Find from sub directory in
 * opened folder, Check whether sub directories or files in the folder is the
 * searched files and Consume them
 */
class Provider : public find::Checker<nwd::fs::path> {
 public:
  virtual ~Provider(){};
  using path = nwd::fs::path;
  using UniqueInputContainer = std::unique_ptr<InputContainer<path>>;
  using UniqueFind = std::unique_ptr<find::Find<path>>;

  /**
   * Open path to iterate
   *
   * this method should not return nullptr
   */
  virtual UniqueInputContainer Open(const path &path) = 0;

  /**
   * Find file in children
   *
   * This method should not return nullptr
   */
  virtual UniqueFind Find(Find *parent, const path &path) = 0;

  /**
   * Process Found file
   */
  virtual void Consume(const path &path) = 0;
};

}  // namespace fs

}  // namespace fmr

#endif  // INCLUDE_FMR_FS_PROVIDER_H_
