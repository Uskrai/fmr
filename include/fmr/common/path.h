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

#ifndef FMR_COMMON_PATH
#define FMR_COMMON_PATH

#include <wx/filename.h>

#include <filesystem>

namespace fmr {

namespace Path {
const std::string EmptyString = "";
const char Separator = std::filesystem::path::preferred_separator;

// // return long path
// wxString GetFullPath( wxString path );
// // return directory name
// wxString GetDirName( wxString path );
// // return parent's path
// wxString GetParent( const wxString& path );
// // strip last separator
// void RemoveDirSep( wxString& path );
// // return Name without separator
// wxString GetName( wxString path );
// // return name with separator if directory
// wxString GetNameWithSep( wxString path );

std::string GetSeparator();

std::string GetName(std::string path);

std::string GetParent(std::string path);

std::string GetRootPath(const std::string &path);

bool HasRootPath(const std::string &path);

std::string GetDirName(const std::string &path);

void RemoveDirSep(std::string &path);

bool IsRoot(const std::string &path);

bool IsChild(const std::string &parent, std::string target);

bool IsAbsolute(const std::string &path);

bool IsRelative(const std::string &path);

std::string Append(const std::string &parent, const std::string &target);

std::string MakeRelative(const std::string &parent, const std::string &target);

std::string MakeAbsolute(const std::string &path);

std::string MakeDirectory(const std::string &path);

}  // namespace Path

}  // namespace fmr
#endif
