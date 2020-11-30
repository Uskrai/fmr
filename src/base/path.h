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

#include <wx/filename.h>
#include <iostream>
namespace Path
{
    void RemoveDirSep( wxString& path );
    const wxUniChar sep = wxFileName::GetPathSeparator();
    
    // return long path
    wxString GetFullPath( wxString path )
    {
        wxFileName name;
        if ( path.StartsWith('.') )
            path = wxFileName::GetCwd() + sep + path.AfterFirst(sep);
    
        if ( name.DirExists(path) )
            name.AssignDir(path);
        else if ( name.FileExists(path) )
            name.Assign(path);

        return name.GetFullPath();
    }

    // return parent's path
    wxString GetParent( const wxString& path )
    {
        wxString name(GetFullPath(path));

        RemoveDirSep(name);

        return name.SubString(0,name.rfind(sep));
    }

    // strip last separator
    void RemoveDirSep( wxString& path )
    {
        if ( path.EndsWith(sep) )
            path.RemoveLast();
    }

    // return Name without separator
    wxString GetName( wxString path )
    {
        int idx;
        RemoveDirSep(path);
        
        wxString name = path.SubString( path.rfind(sep) + 1, -1 );

        return name;
    }

    // return name with separator if directory
    wxString GetNameWithSep( wxString path )
    {
        bool isDir = path.EndsWith(sep);
        if( isDir )
            path.RemoveLast();
        
        wxString name = GetName(path);
        name = isDir ? name + sep : name;
        return name;
    }

    

} // namespace Path