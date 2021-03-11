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

#ifndef FMR_FILE_HANDLER_LOCAL
#define FMR_FILE_HANDLER_LOCAL

#include <fmr/file_handler/handler.h>
#include <fmr/file_handler/local/input.h>
#include <fmr/file_handler/local/output.h>

namespace fmr {

namespace file_handler {

namespace local {

class Handler : public fmr::file_handler::Handler {
 public:
  virtual local::Input *Read() override = 0;
  virtual const local::Input *Read() const override = 0;

  virtual local::Output *Write() override = 0;
  virtual const local::Output *Write() const override = 0;
};

}  // namespace local

}  // namespace file_handler

}  // namespace fmr

#endif /* end of include guard: FMR_FILE_HANDLER_LOCAL */
