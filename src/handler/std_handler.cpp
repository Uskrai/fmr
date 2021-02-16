/*
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
 */

#include "fmr/handler/std_handler.h"

#include <fmr/common/compare.h>
#include <fmr/common/path.h>
#include <wx/log.h>

#include <algorithm>
#include <iostream>

#include "fmr/nowide/fstream.h"

namespace fmr {

STDHandler::STDHandler(const std::string &path) { Open(path); }

void STDHandler::Open(const std::string &path) {
  filename_ = path;
  name_ = Path::GetDirName(path);
  is_opened_ = true;

  std::string parent = Path::GetParent(name_);
  if (!Path::IsRoot(name_)) {
    parent_ = std::make_shared<STDHandler>(STDHandler(parent));
  }
}

STDHandler::STDHandler(STDHandler &&move) {
  name_ = std::move(move.name_);
  filename_ = std::move(move.filename_);
  is_opened_ = std::move(move.is_opened_);
  parent_ = std::move(move.parent_);
  iterator_item_ = std::move(move.iterator_item_);
  iterator_flags_ = std::move(move.iterator_flags_);
  iterator_ = std::move(move.iterator_);
  list_stream_ = std::move(move.list_stream_);
  list_write_stream_ = std::move(move.list_write_stream_);
}

const std::string &STDHandler::GetName() const { return name_; }

const std::shared_ptr<AbstractHandler> STDHandler::GetParent() const {
  return parent_;
}

std::shared_ptr<AbstractHandler> STDHandler::GetParent() { return parent_; }

const std::vector<SStream> &STDHandler::GetChild() const {
  return list_stream_;
}

std::vector<SStream> &STDHandler::GetChild() { return list_stream_; }

std::string STDHandler::GetNext() const { return GetFromCurrent(1); }

std::string STDHandler::GetPrev() const { return GetFromCurrent(-1); }

size_t STDHandler::Size() const { return list_stream_.size(); }

SStream &STDHandler::Item(size_t index) { return list_stream_.at(index); }

const SStream &STDHandler::Item(size_t index) const {
  return list_stream_.at(index);
}

bool STDHandler::IsOpened() const { return is_opened_; }

bool STDHandler::StreamOpenable() { return true; }

bool STDHandler::IsStreamOpenable() const { return StreamOpenable(); }

bool STDHandler::IsExist(size_t idx) const {
  return Vector::IsExist(list_stream_, idx);
}

bool STDHandler::CanHandle(const std::string &path) { return true; }

bool STDHandler::GetStream(SStream &stream) {
  if (stream.GetHandlerPath() != GetName()) return false;

  stream.Open(GetItemPath(stream));
  return true;
}

bool STDHandler::OpenStream(const std::string &path, SStream &stream,
                            bool is_get_stream) {
  stream.SetName(Path::MakeRelative(GetName(), path));
  stream.SetHandlerPath(GetName());
  stream.SetDir(Path::IsDirectory(path));
  stream.Open();

  if (is_get_stream) GetStream(stream);

  return true;
}

bool STDHandler::GetFirst(SStream &stream, DirGetFlags flags,
                          bool is_get_stream) {
  nwd::fs::directory_options options;

  if (!IsOpened() || !nwd::fs::exists(GetName())) return false;

  if (flags & kDirFollowSymLink)
    options |= nwd::fs::directory_options::follow_directory_symlink;

  if (flags & kDirSkipDenied)
    options |= nwd::fs::directory_options::skip_permission_denied;

  iterator_flags_ = flags;
  iterator_item_ = std::unique_ptr<nwd::fs::directory_iterator>(
      new nwd::fs::directory_iterator(GetName(), options));
  iterator_ = nwd::fs::begin(*iterator_item_);
  return GetNextStream(stream, is_get_stream);
}

bool STDHandler::GetNextStream(SStream &stream, bool is_get_stream) {
  if (!iterator_item_ || iterator_ == nwd::fs::end(*iterator_item_))
    return false;

  stream = SStream();
  nwd::fs::path path = iterator_->path();

  OpenStream(Path::MakeString(path), stream, is_get_stream);

  iterator_++;
  return true;
}

std::string STDHandler::GetFromCurrent(int step) const {
  if (!GetParent()) return "";

  size_t idx = GetParent()->Index(GetName());
  if (GetParent()->IsExist(idx + step)) return parent_->GetItemPath(idx + step);

  return "";
}

std::string STDHandler::GetItemPath(const SStream &stream) const {
  return Path::Append(GetName(), stream.GetName());
}

std::string STDHandler::GetItemPath(size_t idx) const {
  return GetItemPath(Item(idx));
}

size_t STDHandler::Index(const std::string &name) const {
  size_t i = 0;
  if (name == GetName()) return 0;

  std::string temp = name;
  Path::RemoveDirSep(temp);

  if (Path::IsAbsolute(temp)) temp = Path::MakeRelative(GetName(), temp);
  for (const auto &it : list_stream_) {
    if (temp == it.GetName()) return i;
    i++;
  }
  return -1;
}

void STDHandler::Traverse(bool is_get_stream, DirGetFlags flags) {
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
  list_stream_ = std::move(vec_stream);
}

bool STDHandler::CreateDirectories() {
  return IsOpened() && CreateDirectories(GetName());
}

bool STDHandler::CreateDirectories(const std::string &path) {
  return path != "" &&
         (nwd::fs::exists(path) || nwd::fs::create_directories(path));
}

bool STDHandler::CreateDirectory(std::string directory_name, bool overwrite) {
  if (!IsOpened()) return false;

  std::string path = Path::Append(GetName(), directory_name);
  if (nwd::fs::exists(path) && !overwrite) return false;

  SStream stream;
  StreamActionType flags = kStreamWrite;

  if (overwrite) flags |= kStreamOverwrite;

  stream.SetHandlerPath(GetName());
  stream.SetName(directory_name);
  stream.SetType(flags);
  stream.SetDir();

  list_write_stream_.push_back(std::move(stream));

  return true;
}

bool STDHandler::CreateFiles(SStream stream, const std::string &file_name,
                             bool overwrite) {
  if (!IsOpened()) return false;

  std::string path = Path::Append(GetName(), file_name);
  if (nwd::fs::exists(path) && !overwrite) return false;

  StreamActionType flags = kStreamWrite;

  if (overwrite) flags |= kStreamOverwrite;

  stream.SetHandlerPath(GetName());
  stream.SetName(file_name);
  stream.SetType(flags);
  list_write_stream_.push_back(std::move(stream));

  return true;
}

bool STDHandler::Remove(const std::string &filename, bool recursive) {
  const std::string path = GetName() + filename;

  size_t index = Index(GetName() + filename);
  if (IsExist(index)) {
    SStream stream;
    StreamActionType flags = kStreamRemove;
    if (recursive) flags |= kStreamRecursive;

    stream.SetName(path);
    stream.SetHandlerPath(GetName());
    stream.SetType(flags);

    list_write_stream_.push_back(std::move(stream));
    return true;
  }
  return false;
}

bool STDHandler::RemoveAll() { return IsOpened() && RemoveAll(GetName()); }

bool STDHandler::RemoveAll(const std::string &path) {
  return !path.empty() && (!nwd::fs::exists(path) || nwd::fs::remove_all(path));
}

bool STDHandler::CommitWrite() {
  if (!nwd::fs::exists(GetName())) return false;

  for (auto &it : list_write_stream_) {
    std::string path = Path::Append(it.GetHandlerPath(), it.GetName());
    if (it.GetType() & kStreamRemove) {
      if (it.GetType() & kStreamRecursive)
        nwd::fs::remove_all(path);
      else
        nwd::fs::remove(path);

      continue;
    } else if (it.GetType() & kStreamWrite) {
      if (it.IsDir()) {
        nwd::fs::create_directories(path);
      } else {
        nwd::fs::path temp(path);
        nwd::ofstream out_stream;
        out_stream.open(Path::MakeString(temp), nwd::ofstream::binary);

        auto buffer = std::make_unique<char[]>(it.GetSize());
        size_t length = it.CopyTo(buffer.get(), it.GetSize());

        out_stream.write(buffer.get(), length);
        out_stream.close();
      }
    }
  }

  list_write_stream_.clear();

  return true;
}

void STDHandler::Reset() {
  Close();
  Clear();
  is_opened_ = false;
  name_ = "";
  filename_ = "";
  parent_ = NULL;
}

void STDHandler::Clear() {
  list_stream_.clear();
  list_write_stream_.clear();
}

void STDHandler::Close() {
  iterator_item_ = NULL;
  iterator_flags_ = kDirNone;
}

}  // namespace fmr
