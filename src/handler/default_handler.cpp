/*
 *
 * Copyright (c) 2020-2021 Uskrai
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include <fmr/common/compare.h>
#include <fmr/common/path.h>
#include <fmr/handler/default_handler.h>
#include <fmr/handler/handler_factory.h>
#include <wx/wfstream.h>

#include <algorithm>

#include "fmr/nowide/string.h"

namespace fmr {

DefaultHandler::DefaultHandler(const std::string &path) { this->Open(path); }

DefaultHandler::DefaultHandler(DefaultHandler &&handler) {
  using std::move;
  opened_name_ = move(handler.opened_name_);
  parent_handler_ = move(handler.parent_handler_);
  m_all = move(handler.m_all);
  list_write_stream_ = move(handler.list_write_stream_);
}

DefaultHandler::DefaultHandler(const DefaultHandler &handler) {
  if (handler.is_opened_) Open(Path::MakeDirectory(handler.GetName()));

  m_filename = handler.m_filename;
  m_all = handler.m_all;
  list_write_stream_ = handler.list_write_stream_;
}

void DefaultHandler::Open(const std::string &path) {
  opened_name_ = Path::GetDirName(path);
  is_opened_ = true;

  std::string parent = Path::GetParent(GetName());
  if (!Path::IsRoot(GetName())) {
    parent_handler_ =
        std::shared_ptr<DefaultHandler>(new DefaultHandler(parent));
  }
}

const std::string &DefaultHandler::GetName() const { return opened_name_; }

const std::shared_ptr<AbstractHandler> DefaultHandler::GetParent() const {
  return parent_handler_;
}

std::shared_ptr<AbstractHandler> DefaultHandler::GetParent() {
  return parent_handler_;
}

const std::vector<SStream> &DefaultHandler::GetChild() const { return m_all; }

std::vector<SStream> &DefaultHandler::GetChild() { return m_all; }

const SStream &DefaultHandler::Item(size_t idx) const { return m_all.at(idx); }

SStream &DefaultHandler::Item(size_t idx) { return m_all.at(idx); }

size_t DefaultHandler::Size() const { return GetChild().size(); }

bool DefaultHandler::IsExist(size_t idx) const {
  return Vector::IsExist(GetChild(), idx);
}

bool DefaultHandler::IsDirectory(const SStream &stream) const {
  return Path::IsDirectory(GetItemPath(stream));
}

bool DefaultHandler::IsOpened() const { return is_opened_; }

std::string DefaultHandler::GetNext() const { return GetFromCurrent(1); }

std::string DefaultHandler::GetPrev() const { return GetFromCurrent(-1); }

std::string DefaultHandler::GetItemPath(const SStream &stream) const {
  return Path::Append(stream.GetHandlerPath(), stream.GetName());
}

std::string DefaultHandler::GetItemPath(size_t index) const {
  return GetItemPath(Item(index));
}

std::string DefaultHandler::GetFromCurrent(int step) const {
  if (!GetParent()) return Path::EmptyString;

  size_t idx = GetParent()->Index(GetName());

  if (GetParent()->IsExist(idx + step))
    return parent_handler_->GetItemPath(idx + step);

  return Path::EmptyString;
}

size_t DefaultHandler::Index(const std::string &path) const {
  if (path == GetName()) return 0;

  std::string temp = path;
  if (Path::IsAbsolute(temp)) temp = Path::MakeRelative(GetName(), path);

  Path::RemoveDirSep(temp);

  size_t idx = 0;
  while (Vector::IsExist(m_all, idx)) {
    if (m_all.at(idx).GetName() == temp) return idx;
    idx++;
  }
  return -1;
}

void DefaultHandler::Traverse(bool is_get_stream, DirGetFlags flags) {
  SStream stream;
  std::vector<SStream> vec_stream;

  bool cont = GetFirst(stream, flags, is_get_stream);

  vec_stream.reserve(Size());
  while (cont) {
    vec_stream.push_back(stream);
    cont = GetNextStream(stream, is_get_stream);
  }

  vec_stream.shrink_to_fit();
  std::sort(vec_stream.begin(), vec_stream.end(), Compare::NaturalSortable);
  m_all = std::move(vec_stream);
}

void DefaultHandler::OpenStream(const std::string &filename, SStream &stream,
                                bool is_get_stream) {
  stream.SetName(filename);
  stream.SetHandlerPath(GetName());
  stream.SetDir(wxFileName::DirExists(stream.GetName()));

  if (is_get_stream) GetStream(stream);
}

bool DefaultHandler::GetFirst(SStream &stream, DirGetFlags flags,
                              bool is_get_stream) {
  if (GetName() == "") return false;

  wxString name = String::Widen<wxString>(GetName());
  opened_directory_.Open(name);

  if (!opened_directory_.IsOpened()) return false;

  int flag_dir = wxDIR_HIDDEN;
  if (flags & kDirFile) flag_dir += wxDIR_FILES;

  if (flags & kDirDirectory) flag_dir += wxDIR_DIRS;

  if (!(flags & kDirFollowSymLink)) flag_dir += wxDIR_NO_FOLLOW;

  wxString filename;
  if (!opened_directory_.GetFirst(&filename, wxEmptyString, flag_dir))
    return false;

  OpenStream(String::Narrow(filename), stream, is_get_stream);
  return true;
}

bool DefaultHandler::GetNextStream(SStream &stream, bool is_get_stream) {
  wxString filename;

  if (!opened_directory_.IsOpened()) return false;

  if (!opened_directory_.GetNext(&filename)) return false;

  OpenStream(String::Narrow(filename), stream, is_get_stream);
  return true;
}

bool DefaultHandler::GetStream(SStream &stream) {
  if (stream.GetHandlerPath() != GetName()) return false;

  std::string path = Path::Append(stream.GetHandlerPath(), stream.GetName());

  stream.Open(path);
  return true;
}

bool DefaultHandler::CreateDirectories() {
  return IsOpened() && CreateDirectories(GetName());
}

bool DefaultHandler::CreateDirectories(const std::string &path) {
  return path != "" && (wxDir::Exists(path) ||
                        wxDir::Make(path, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL));
}

bool DefaultHandler::CreateDirectory(const std::string &directory_name,
                                     bool overwrite) {
  if (!IsOpened()) return false;

  if (!Path::IsChild(GetName(), directory_name)) return false;

  SStream stream;
  StreamActionType flags = kStreamWrite;

  if (overwrite) flags |= kStreamOverwrite;

  stream.SetName(directory_name);
  stream.SetHandlerPath(GetName());
  stream.SetDir();
  stream.SetType(flags);

  list_write_stream_.push_back(std::move(stream));
  return true;
}

bool DefaultHandler::CreateFiles(SStream stream, const std::string &filename,
                                 bool overwrite) {
  if (!IsOpened()) return false;

  if (!Path::IsChild(GetName(), filename)) return false;

  StreamActionType flags = kStreamWrite;

  if (overwrite) flags |= kStreamOverwrite;

  stream.SetName(filename);
  stream.SetHandlerPath(GetName());
  stream.SetType(flags);

  list_write_stream_.push_back(std::move(stream));
  return true;
}

bool DefaultHandler::Remove(const std::string &name, bool recursive) {
  if (!IsOpened()) return false;

  if (!Path::IsChild(GetName(), name)) return false;

  SStream stream;
  StreamActionType flags = kStreamRemove;

  if (recursive) flags |= kStreamRecursive;

  stream.SetName(name);
  stream.SetHandlerPath(GetName());
  stream.SetType(flags);

  list_write_stream_.push_back(std::move(stream));
  return true;
}

bool DefaultHandler::RemoveAll() { return IsOpened() && RemoveAll(GetName()); }

bool DefaultHandler::RemoveAll(const std::string &path) {
  return !path.empty() && (!wxFileName::Exists(path) ||
                           wxDir::Remove(path, wxPATH_RMDIR_RECURSIVE));
}

void DefaultHandler::DoCreateFile(const SStream &stream) {
  auto path = String::Widen<wxString>(
      Path::Append(stream.GetHandlerPath(), stream.GetName()));

  wxTempFileOutputStream out_stream(path);
  auto buffer = std::make_unique<char[]>(stream.GetSize());

  size_t length = stream.CopyTo(buffer.get(), stream.GetSize());

  out_stream.Write(buffer.get(), length);
  out_stream.Commit();
}

void DefaultHandler::DoRemove(const SStream &stream) {
  int flags =
      (stream.GetType() & kStreamRecursive) ? wxPATH_RMDIR_RECURSIVE : 0;

  wxDir::Remove(stream.GetHandlerPath() + stream.GetName(), flags);
}

void DefaultHandler::DoCreateDirectory(const SStream &stream) {
  int flags = (stream.GetType() & kStreamRecursive) ? wxPATH_MKDIR_FULL : 0;

  auto path = String::Widen<wxString>(
      Path::Append(stream.GetHandlerPath(), stream.GetName()));
  wxDir::Make(path, wxS_DIR_DEFAULT, flags);
}

bool DefaultHandler::CommitWrite() {
  if (!wxDir::Exists(GetName())) return false;

  for (const auto &it : list_write_stream_) {
    if (it.GetType() & kStreamRead) continue;

    if (wxFileName::Exists(it.GetHandlerPath() + it.GetName())) {
      if (it.GetType() & kStreamOverwrite)
        DoRemove(it);
      else
        continue;
    }

    if (it.IsDir())
      DoCreateDirectory(it);
    else if (it.GetType() & kStreamWrite)
      DoCreateFile(it);
    else if (it.GetType() & kStreamRemove)
      DoRemove(it);
  }

  list_write_stream_.clear();

  return true;
}

void DefaultHandler::Reset() {
  Clear();
  Close();

  list_write_stream_.clear();
  opened_name_ = Path::EmptyString;
  is_opened_ = false;
  parent_handler_ = NULL;
}

void DefaultHandler::Clear() {
  m_all.clear();
  list_write_stream_.clear();
}

void DefaultHandler::Close() {
  Clear();
  opened_directory_.Close();
}

DefaultHandler::~DefaultHandler() { Close(); }

}  // namespace fmr
