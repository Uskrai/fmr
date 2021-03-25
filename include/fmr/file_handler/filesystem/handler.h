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
#include <fmr/file_handler/local/handler.h>

#include <memory>

namespace fmr {

namespace file_handler {

namespace filesystem {

class Handler : public local::Handler {
  Input input_;
  Output output_;
  std::string path_;

  std::unique_ptr<Handler> parent_;

 public:
  Handler();
  Handler(const std::string &path) { Open(path); }

  virtual bool Open(const std::string &path) override;
  virtual bool Open(const file_handler::ReadStream &path) override;
  virtual bool Open(const local::ReadStream &path) override;

  virtual std::string GetPath() const override { return path_; }

  virtual std::string GetInternalName(const std::string &path) const override;
  virtual std::string GetExternalName(
      const file_handler::ReadStream &stream) const override;

  virtual Output *Write() override { return &output_; }
  virtual const Output *Write() const override { return &output_; }

  virtual Input *Read() override { return &input_; }
  virtual const Input *Read() const override { return &input_; }

  virtual Handler *GetParent() override { return parent_.get(); }

  virtual bool IsExist() const override;

  virtual bool IsOk() const override;
  virtual bool IsHandleable(const std::string &name) const override {
    return CanHandle(name);
  }

  virtual bool IsHandleable(const file_handler::Stream &stream) const override {
    return true;
  }

  static bool CanHandle(const std::string &name) { return true; }

 protected:
  Handler *DoCreateNew() const override { return new Handler(); }
};

}  // namespace filesystem

}  // namespace file_handler

}  // namespace fmr

#endif /* end of include guard: FMR_FILE_HANDLER_FILESYSTEM_FILE_HANDLER */
