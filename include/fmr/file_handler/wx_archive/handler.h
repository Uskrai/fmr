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

#include <fmr/file_handler/factory.h>
#include <fmr/file_handler/handler.h>
#include <fmr/file_handler/wx_archive/input.h>
#include <fmr/file_handler/wx_archive/output.h>
#include <wx/archive.h>

namespace fmr {

namespace file_handler {

namespace wx_archive {

using HandlerBase = fmr::file_handler::Handler;
class Handler : public HandlerBase {
  std::string path_;
  std::shared_ptr<file_handler::ReadStream> stream_;

  using ParentHandler = file_handler::Handler;
  using UniqueParentHandler = std::unique_ptr<ParentHandler>;
  UniqueParentHandler parent_;
  Input input_;
  Output output_;

  const Factory *factory_ = GetHandlerFactory();
  const wxArchiveClassFactory *archive_factory_ = nullptr;

 public:
  Handler() : input_(this), output_(this, &input_) {}
  Handler(const std::string &path) : Handler() { Open(path); }
  Handler(const file_handler::ReadStream &stream) : Handler() { Open(stream); }

  bool Open(const std::string &path) override;
  bool Open(const file_handler::ReadStream &stream) override;
  bool Open(UniqueParentHandler handler, const std::string &path);
  std::string GetPath() const override { return path_; }

  std::string GetInternalName(const std::string &path) const override;
  std::string GetExternalName(
      const file_handler::ReadStream &stream) const override;

  virtual file_handler::Handler *GetParent() override { return parent_.get(); }

  bool IsOk() const override;
  bool IsExist() const override;

  Input *Read() override { return &input_; }
  const Input *Read() const override { return &input_; }

  Output *Write() override { return &output_; }
  const Output *Write() const override { return &output_; }

  virtual bool IsHandleable(const Stream &stream) const override;
  virtual bool IsHandleable(const file_handler::ReadStream &stream) const;
  virtual bool IsHandleable(const std::string &path) const override;

  static const wxArchiveClassFactory *FindFactory(const std::string &path);
  static const wxArchiveClassFactory *FindFactory(
      const file_handler::ReadStream &stream);

  static const Factory *GetHandlerFactory();

  const Factory *GetFactory() { return factory_; }

  void UpdateStream(std::shared_ptr<file_handler::ReadStream> stream);

 private:
  Handler *DoCreateNew() const override { return new Handler(); }

  bool DoOpen(std::unique_ptr<file_handler::Handler> parent,
              std::shared_ptr<file_handler::ReadStream> stream,
              std::string path);

  void Reset();
};

}  // namespace wx_archive

}  // namespace file_handler

}  // namespace fmr
