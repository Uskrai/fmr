/*
 *  Copyright (c) 2020-2021 Uskrai
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

#include "fmr/common/path.h"

#include <boost/filesystem/file_status.hpp>
#include <boost/filesystem/operations.hpp>
#include <iostream>

#include "fmr/nowide/filesystem.h"

namespace fmr {

namespace Path {

std::string MakeString(const nwd::fs::path &path) { return path.string(); }

std::string MakeString(const wxString &path) {
  return std::string(path.ToUTF8());
}

auto MakePath(const std::string &path) { return nwd::fs::path(path); }

char GetSeparator() { return nwd::fs::path::preferred_separator; }
// std::string GetSeparator() { return std::string(1, Separator); }

std::string GetParent(std::string path) {
  if (!IsRoot(path)) RemoveDirSep(path);

  auto temp = MakePath(path);
  auto temp_parent = temp.parent_path();

  return GetDirName(MakeString(temp_parent));
}

std::string GetName(std::string path) {
  RemoveDirSep(path);

  size_t idx = path.rfind(GetSeparator());
  if (idx != std::string::npos) path = path.substr(idx);
  return path;
}

std::string GetRootPath(const std::string &path) {
  return MakeString(MakePath(path).root_path());
}

#ifdef FMR_USE_BOOST_FILESYSTEM
bool IsDirectory(const nwd::fs::path &path) {
  if (nwd::fs::is_directory(path)) return true;

  auto type = nwd::fs::status(path).type();
  if (type == nwd::fs::file_type::reparse_file && path.size() > 8) {
    // temporary fix, cannot find anything to test reparse file in boost doc
    try {
      auto temp_iterator = nwd::fs::directory_iterator(path);
      return true;
    } catch (std::exception &err) {
      return false;
    }
  }
  return false;
}

#else
bool IsDirectory(const nwd::fs::path &path) {
  return nwd::fs::is_directory(path);
}
#endif

bool IsDirectory(const std::string &path) {
  return IsDirectory(MakePath(path));
}

std::string GetDirName(const std::string &path) {
  try {
    if (IsDirectory(path)) {
      return path;
    }
    nwd::fs::path temp = MakePath(path);

    temp.remove_filename();

    if (temp.empty()) return MakeString(nwd::fs::current_path());

    return MakeString(temp);
  } catch (std::exception &err) {
    printf("%s\n", err.what());
  }
  return path;
}

void RemoveDirSep(std::string &path) {
  if (!path.empty() && path.back() == GetSeparator()) path.pop_back();
}

bool HasRootPath(const std::string &path) {
  return MakePath(path).has_root_path();
}

bool IsRoot(const std::string &path) { return GetRootPath(path) == path; }

bool IsChild(const std::string &parent, std::string target) {
  std::string parent_root = GetRootPath(parent);
  std::string target_root = GetRootPath(target);

  if (!HasRootPath(target)) return true;

  if (target_root != parent_root) return false;

  if (parent_root == parent && parent_root == target_root) return true;

  while (target != target_root) {
    target = GetParent(target);
    if (target == parent) return true;
  }

  return false;
}

bool IsAbsolute(const std::string &path) {
  return MakePath(path).is_absolute();
}

bool IsRelative(const std::string &path) {
  return MakePath(path).is_relative();
}

std::string Append(const std::string &parent, const std::string &target) {
  nwd::fs::path path = MakePath(parent);
  return MakeString(path / target);
}

std::string MakeRelative(const std::string &parent, const std::string &target) {
  return MakeString(nwd::fs::relative(MakePath(target), MakePath(parent)));
}

std::string MakeAbsolute(const std::string &path) {
  return MakeString(nwd::fs::absolute(MakePath(path)));
}

std::string MakeDirectory(const std::string &path) {
  nwd::fs::path temp = MakePath(path);

  if (nwd::fs::is_directory(temp)) return path;

  return Append(path, std::string(1, GetSeparator()));
}
}  // namespace Path

};  // namespace fmr
