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
#include <fmr/file_handler/filesystem/output.h>
#include <fmr/nowide/filesystem.h>
#include <fmr/nowide/fstream.h>

#include <filesystem>

namespace fmr {

namespace file_handler {

namespace filesystem {

void Output::Create() {
  if (!Path::Exist(GetName()))
    nwd::fs::create_directories(Path::MakePath(GetName()));
}

void Output::CommitWrite() {
  for (auto &it : write_vec_) {
    if (!(it.GetWriteType() & kWriteOverwrite))
      if (Path::Exist(it.GetFullPath())) continue;

    if (it.GetWriteType() & kWriteDirectory) {
      DoCreateDirectory(it.GetName());
      continue;
    } else {
      nwd::ofstream stream(it.GetFullPath());
      stream.write(reinterpret_cast<const char *>(it.GetBuffer()), it.Size());
      stream.close();
    }
  }
  write_vec_.clear();
}

void Output::CreateDirectory(const std::string &name) {
  auto path = Path::MakePath(Path::Append(GetName(), name));
  CreateFile(name, nullptr, kWriteDirectory);
}

void Output::DeleteDirectory(const std::string &name) { ; }

void Output::DeleteFile(const std::string &name) { ; }

void Output::Delete() { nwd::fs::remove_all(Path::MakePath(GetName())); }

WriteStream *Output::CreateFile(const std::string &name,
                                file_handler::Stream *stream, WriteType type) {
  auto &it = write_vec_.emplace_back(GetName(), name, type);
  if (stream) it.Write(*stream);
  return &it;
}

bool Output::DoCreateDirectory(const std::string &name) { return false; }

}  // namespace filesystem

}  // namespace file_handler

}  // namespace fmr
