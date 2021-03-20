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

#include <fmr/file_handler/handler.h>
#include <fmr/file_handler/local/handler.h>
#include <fmr/file_handler/wx_archive/input.h>
#include <wx/archive.h>

namespace fmr {

namespace file_handler {

namespace wx_archive {

using HandlerBase = fmr::file_handler::Handler;
class Handler : public HandlerBase {
  std::string path_;
  std::shared_ptr<file_handler::ReadStream> stream_;

  std::unique_ptr<local::Handler> parent_;
  Input input;

 public:
  void Open(const std::string &path) override;
  void Open(const file_handler::ReadStream &stream);
  std::string GetPath() const override { return path_; }

  virtual local::Handler *GetParent() override { return parent_.get(); }

  bool IsOk() const override;
  bool IsExist(const std::string &path) const override;

  Input *Read() override { return nullptr; }
  const Input *Read() const override { return nullptr; }

  Output *Write() override { return nullptr; }
  const Output *Write() const override { return nullptr; }

  virtual bool IsHandleable(const Stream &stream) const override;
  virtual bool IsHandleable(const file_handler::ReadStream &stream) const;
  virtual bool IsHandleable(const std::string &path) const override;

  static const wxArchiveClassFactory *FindFactory(const std::string &path);
  static const wxArchiveClassFactory *FindFactory(
      const file_handler::ReadStream &stream);
};

}  // namespace wx_archive

}  // namespace file_handler

}  // namespace fmr
