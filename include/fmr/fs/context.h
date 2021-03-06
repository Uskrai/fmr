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

#ifndef INCLUDE_FMR_FS_CONTEXT_H_
#define INCLUDE_FMR_FS_CONTEXT_H_

#include <fmr/find/context.h>
#include <fmr/find/find.h>
#include <fmr/nowide/fs.h>

#include <memory>

namespace fmr {

namespace fs {

class Find;

class Context : public find::Context<nwd::fs::path> {
 public:
  virtual std::unique_ptr<find::Find<>> Find(Find *parent,
                                             const nwd::fs::path &path) = 0;
};

}  // namespace fs

}  // namespace fmr

#endif  // INCLUDE_FMR_FS_CONTEXT_H_
