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
    wxString GetFullPath( const wxString& path )
    {
        wxFileName name;
        if ( path.StartsWith('.') )
            name = wxFileName::GetCwd() + sep + path.AfterFirst(sep);
    
        if ( name.DirExists(path) )
            name.AssignDir(path);
        else if ( name.FileExists(path) )
            name.Assign(path);

        return name.GetFullPath();
    }

    wxString GetParent( const wxString& path )
    {
        wxString name(GetFullPath(path));

        RemoveDirSep(name);

        return name.SubString(0,name.rfind(sep));
    }

    void RemoveDirSep( wxString& path )
    {
        if ( path.EndsWith(sep) )
            path.RemoveLast();
    }

    wxString GetNameWithSep( wxString& path )
    {
        int idx;
        if ( path.EndsWith(sep) )
            idx = path.rfind(sep, 1);
        else idx = path.rfind(sep);
        return path.SubString(idx,-1);
    }

    wxString GetName( const wxString& path )
    {
        int idx;
        idx = path.find_last_not_of(sep);
        
        wxString name = path.SubString( path.rfind(sep,idx) + 1, -1 );
        std::cout << name << '\n';
        return name;
        
        // return name.GetPath();
        // GetNameWithSep(name);
        // return name.BeforeLast(sep);
    }

} // namespace Path