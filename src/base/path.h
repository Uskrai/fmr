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
#include <iostream>
inline namespace Path
{
    const wxUniChar sep = wxFileName::GetPathSeparator();
    
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

} // namespace Path

#endif