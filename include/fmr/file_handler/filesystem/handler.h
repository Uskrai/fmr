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

#ifndef FMR_FILE_HANDLER_FILESYSTEM_FILE_HANDLER
#define FMR_FILE_HANDLER_FILESYSTEM_FILE_HANDLER

#include <fmr/file_handler/filesystem/input.h>
#include <fmr/file_handler/filesystem/output.h>
#include <fmr/file_handler/filesystem/stream.h>
#include <fmr/file_handler/local/handler.h>

#include <memory>

namespace fmr {

namespace file_handler {

namespace filesystem {

class Handler : public local::Handler {
  std::unique_ptr<Input> input_;
  std::unique_ptr<Output> output_;
  std::string path_;

 public:
  Handler() {
    input_ = std::make_unique<Input>();
    // output_ = std::make_unique<Output>();
  }

  virtual void Open(const std::string &path) override;
  virtual std::string GetPath() const override { return path_; }

  virtual local::Output *Write() override { return output_.get(); }
  virtual const local::Output *Write() const override { return output_.get(); }

  virtual Input *Read() override { return input_.get(); }
  virtual const Input *Read() const override { return input_.get(); }

  virtual bool IsOk() const override { return true; }
  virtual bool IsHandleable(const std::string &name) { return CanHandle(name); }

  static bool CanHandle(const std::string &name) { return true; }
};

}  // namespace filesystem

}  // namespace file_handler

}  // namespace fmr

#endif /* end of include guard: FMR_FILE_HANDLER_FILESYSTEM_FILE_HANDLER */
