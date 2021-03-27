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

#include <fmr/common/path.h>
#include <fmr/file_handler/filesystem/handler.h>
#include <fmr/file_handler/filesystem/output.h>
#include <fmr/nowide/filesystem.h>
#include <fmr/nowide/fstream.h>

#include "fmr/file_handler/write_type.h"

namespace fmr {

namespace file_handler {

namespace filesystem {

void Output::Create() {
  if (!Path::Exist(GetName()))
    nwd::fs::create_directories(Path::MakePath(GetName()));
}

void Output::CommitWrite() {
  Handler handler{};
  handler.Open(name_);
  handler.Read()->Traverse(false);

  for (auto &it : write_vec_) {
    if (it.GetWriteType() & kWriteDelete) {
      DoDelete(it);

      continue;
    } else {
      DoWrite(it);
    }
  }
  write_vec_.clear();
}

void Output::CreateDirectory(const std::string &name) {
  auto path = Path::MakePath(Path::Append(GetName(), name));
  CreateFile(name, nullptr, kWriteDirectory);
}

void Output::DeleteDirectory(const std::string &name) {
  CreateFile(name, nullptr, kWriteDelete | kWriteDirectory);
}

void Output::DeleteFile(const std::string &name) {
  CreateFile(name, nullptr, kWriteDelete);
}

void Output::Delete() { nwd::fs::remove_all(Path::MakePath(GetName())); }

WriteStream *Output::CreateFile(const std::string &name,
                                file_handler::Stream *stream, WriteType type) {
  auto &it = write_vec_.emplace_back(GetName(), name, type);
  if (stream) it.Write(*stream);
  return &it;
}

bool Output::DoWrite(const WriteStream &stream) {
  if (stream.GetWriteType() & kWriteDirectory) {
    return DoWriteDirectory(stream);
  } else
    return DoWriteFile(stream);
}

bool Output::DoWriteDirectory(const WriteStream &stream) {
  return nwd::fs::create_directory(stream.GetFullPath());
}

bool Output::DoWriteFile(const WriteStream &stream) {
  nwd::ofstream w_stream(stream.GetFullPath());
  w_stream.write(reinterpret_cast<const char *>(stream.GetBuffer()),
                 stream.Size());
  w_stream.close();
  return w_stream.good();
}

bool Output::DoDelete(const WriteStream &stream) {
  if (stream.GetWriteType() & kWriteDirectory) {
    return DoDeleteDirectory(stream);
  } else
    return DoDeleteFile(stream);
}

bool Output::DoDeleteDirectory(const WriteStream &stream) {
  return Path::IsDirectory(stream.GetFullPath()) &&
         nwd::fs::is_empty(stream.GetFullPath()) &&
         nwd::fs::remove(stream.GetFullPath());
}

bool Output::DoDeleteFile(const WriteStream &stream) {
  return !Path::IsDirectory(stream.GetFullPath()) &&
         nwd::fs::remove(stream.GetFullPath());
}

}  // namespace filesystem

}  // namespace file_handler

}  // namespace fmr
