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

#include <fmr/common/path.h>

#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

namespace fmr {

namespace Path {

std::string MakeString(const fs::path &path) { return path.u8string(); }

std::string MakeString(const wxString &path) {
  return std::string(path.ToUTF8());
}

fs::path MakePath(const std::string &path) { return fs::u8path(path); }

std::string GetSeparator() { return std::string(1, Separator); }

std::string GetParent(std::string path) {
  if (!IsRoot(path)) RemoveDirSep(path);

  fs::path temp = MakePath(path);
  fs::path temp_parent = temp.parent_path();

  return GetDirName(MakeString(temp_parent));
}

std::string GetName(std::string path) {
  RemoveDirSep(path);

  size_t idx = path.rfind(Separator);
  if (idx != std::string::npos) path = path.substr(idx);
  return path;
}

std::string GetRootPath(const std::string &path) {
  return MakeString(fs::path(path).root_path());
}

std::string GetDirName(const std::string &path) {
  fs::path temp = MakePath(path);

  if (fs::is_directory(temp)) return MakeString(temp);

  temp.remove_filename();

  if (temp.empty()) return MakeString(fs::current_path());

  return MakeString(temp);
}

void RemoveDirSep(std::string &path) {
  if (!path.empty() && path.back() == Separator) path.pop_back();
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
  fs::path path = MakePath(parent);
  return MakeString(path / target);
}

std::string MakeRelative(const std::string &parent, const std::string &target) {
  return MakeString(fs::relative(MakePath(target), MakePath(parent)));
}

std::string MakeAbsolute(const std::string &path) {
  return MakeString(fs::absolute(MakePath(path)));
}

std::string MakeDirectory(const std::string &path) {
  fs::path temp = MakePath(path);

  if (fs::is_directory(temp)) return path;

  return Append(path, GetSeparator());
}
}  // namespace Path

};  // namespace fmr
