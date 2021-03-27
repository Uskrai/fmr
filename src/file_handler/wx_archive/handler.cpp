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

#include "fmr/file_handler/wx_archive/handler.h"

#include "fmr/common/path.h"
#include "fmr/file_handler/factory.h"
#include "fmr/nowide/string.h"
#include "wx/archive.h"

namespace fmr {

namespace file_handler {

namespace wx_archive {

std::string PopExtension(std::string path) {
  auto fcf = wxFilterClassFactory::Find(path, wxSTREAM_FILEEXT);

  if (fcf) path = fcf->PopExtension(path);

  auto factory = Handler::FindFactory(path);

  if (factory) path = factory->PopExtension(path);
  return path;
}

bool Handler::Open(const std::string &path) {
  Reset();

  if (factory_) {
    auto handler = factory_->NewHandler(PopExtension(path));
    if (handler && handler->GetParent()) {
      auto parent = handler->GetParent()->CreateNew();
      parent->Open(handler->GetParent()->GetPath());
      return Open(std::move(parent), path);
    }
  }
  return false;
}

bool Handler::Open(const file_handler::ReadStream &stream) {
  Reset();

  auto ret = FindFactory(stream);
  if (ret && factory_) {
    auto parent = factory_->NewHandler(stream.GetHandlerPath());
    return factory_ && parent &&
           DoOpen(factory_->NewHandler(stream.GetHandlerPath()), stream.Clone(),
                  parent->GetExternalName(stream));
  }
  return ret;
}

bool Handler::Open(Handler::UniqueParentHandler handler,
                   const std::string &path) {
  Reset();

  if (handler) {
    handler->Read()->Traverse(false);
    auto idx = handler->Read()->Index(path);

    std::shared_ptr<file_handler::ReadStream> stream;
    if (idx < handler->Read()->Size())
      stream = handler->Read()->At(idx)->Clone();
    else
      stream = std::make_shared<
          utility::ReadMemoryStreamHelper<file_handler::ReadStream>>(
          handler->GetPath(), handler->GetInternalName(path));

    DoOpen(std::move(handler), std::move(stream), path);
  }
  return false;
}

bool Handler::DoOpen(std::unique_ptr<file_handler::Handler> parent,
                     std::shared_ptr<file_handler::ReadStream> stream,
                     std::string path) {
  archive_factory_ = FindFactory(stream->GetName());
  stream_ = stream;
  path_ = path;
  Read()->Open(stream);
  Write()->Open(stream, archive_factory_, parent.get());
  parent_ = std::move(parent);
  return stream_ && archive_factory_ && parent_;
}

std::string Handler::GetInternalName(const std::string &path) const {
  return "";
}

std::string Handler::GetExternalName(
    const file_handler::ReadStream &stream) const {
  return "";
}

bool Handler::IsHandleable(const std::string &str_path) const {
  return FindFactory(str_path);
}

bool Handler::IsHandleable(const Stream &stream) const { return false; }

bool Handler::IsHandleable(const file_handler::ReadStream &stream) const {
  return FindFactory(stream);
}

const wxArchiveClassFactory *Handler::FindFactory(
    const file_handler::ReadStream &stream) {
  return FindFactory(stream.GetName());
}

const wxArchiveClassFactory *Handler::FindFactory(const std::string &str_path) {
  const wxArchiveClassFactory *factory = nullptr;
  wxString path = String::Widen<wxString>(str_path);

  factory = wxArchiveClassFactory::Find(path, wxSTREAM_FILEEXT);
  if (!factory) {
    const wxFilterClassFactory *fcf;
    fcf = wxFilterClassFactory::Find(path, wxSTREAM_FILEEXT);

    if (fcf) {
      path = fcf->PopExtension(path);
      factory = wxArchiveClassFactory::Find(path, wxSTREAM_FILEEXT);
    }
  }

  return factory;
}

void Handler::UpdateStream(std::shared_ptr<file_handler::ReadStream> stream) {
  stream_ = stream;
  Read()->UpdateStream(stream);
}

bool Handler::IsOk() const { return IsExist() && IsHandleable(path_); }

void Handler::Reset() {
  archive_factory_ = nullptr;
  parent_ = nullptr;
  path_ = "";
  stream_ = nullptr;

  Read()->Clear();
  // Write()->Clear();
}

bool Handler::IsExist() const {
  bool ret = false;
  if (parent_ && parent_->IsExist() && stream_) {
    auto stream = parent_->Read()->GetFirst(false);
    while (stream && !ret) {
      ret = stream->GetName() == stream_->GetName();
      stream = parent_->Read()->GetNext(false);
    }
  }
  return ret;
}

const Factory *Handler::GetHandlerFactory() { return Factory::GetGlobal(); }

}  // namespace wx_archive

}  // namespace file_handler

}  // namespace fmr
