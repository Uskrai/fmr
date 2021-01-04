/*
 *  Copyright (c) 2020 Uskrai
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

#ifndef FMR_BASE_PATH
#define FMR_BASE_PATH

#include <wx/filename.h>

namespace fmr
{

inline namespace Path
{
    const wxUniChar Separator = wxFileName::GetPathSeparator(); 
    
    // return long path
    wxString GetFullPath( wxString path );
    // return directory name
    wxString GetDirName( wxString path );
    // return parent's path
    wxString GetParent( const wxString& path );
    // strip last separator
    void RemoveDirSep( wxString& path );
    // return Name without separator
    wxString GetName( wxString path );
    // return name with separator if directory
    wxString GetNameWithSep( wxString path );

    std::wstring GetParent( std::wstring path );

    std::wstring GetRootPath( const std::wstring &path );

    bool HasRootPath( const std::wstring &path );

    std::wstring GetDirName( const std::wstring &path );

    bool IsRoot( const std::wstring &path );

    bool IsChild( const std::wstring &parent, std::wstring target );

    bool IsAbsolute( const std::wstring &path );

    bool IsRelative( const std::wstring &path );

    std::wstring Append( const std::wstring &parent, const std::wstring &target );

    std::wstring MakeRelative( const std::wstring &parent, const std::wstring &target );

} // namespace Path

} // namespace fmr
#endif