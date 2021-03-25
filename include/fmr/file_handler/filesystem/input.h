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

#ifndef FMR_FILE_HANDLER_FILESYSTEM_INPUT
#define FMR_FILE_HANDLER_FILESYSTEM_INPUT

#include <fmr/file_handler/filesystem/read_stream.h>
#include <fmr/file_handler/input_implementer_helper.h>
#include <fmr/file_handler/local/input.h>

namespace fmr {

namespace file_handler {

namespace filesystem {

using InputBase = InputImplementHelper<ReadStream, local::Input>;
class Input : public InputBase {
  std::unique_ptr<nwd::fs::directory_iterator> iterator_item_;
  bool is_opened_ = false;
  std::string path_;

 public:
  void Open(const std::string &path) { path_ = path; }

  virtual std::string GetPath() const { return path_; }
  virtual ReadStream *GetFirst(bool get_buffer) override;
  virtual ReadStream *GetNext(bool get_buffer) override;

  // virtual bool IsEmpty() const override { return vec_.empty(); };
  virtual bool IsOpened() const override { return is_opened_; }
  // virtual size_t Size() const override { return vec_.size(); };

  virtual size_t Index(const std::string &path) const override;

  virtual void Clear() override;
};

}  // namespace filesystem

}  // namespace file_handler

}  // namespace fmr

#endif /* end of include guard: FMR_FILE_HANDLER_FILESYSTEM_INPUT */
