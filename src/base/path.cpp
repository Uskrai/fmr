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

#include "base/path.h"

namespace fmr
{

namespace Path
{
    wxString GetFullPath( wxString path )
    {
        wxFileName name(path);
        if ( path.StartsWith('.') )
            path = wxFileName::GetCwd() + Separator + path.AfterFirst( Separator );
    
        RemoveDirSep(path);
        if ( name.DirExists(path) )
            name.AssignDir(path);
        else if ( name.FileExists(path) )
            name.Assign(path);

        return name.GetFullPath();
    }

    wxString GetDirName( wxString path )
    {
        path = GetFullPath(path);
        if ( path.EndsWith(Separator) )
            return path;

        size_t idx = path.rfind( Separator );
        return path.SubString( 0, idx );
    }

    // return parent's path
    wxString GetParent( const wxString& path )
    {
        wxString name(GetFullPath(path));

        RemoveDirSep(name);

        return name.SubString(0,name.rfind(Separator));
    }

    // strip last Separatorarator
    void RemoveDirSep( wxString& path )
    {
        if ( path.EndsWith(Separator) )
            path.RemoveLast();
    }

    // return Name without Separatorarator
    wxString GetName( wxString path )
    {
        RemoveDirSep(path);
        
        wxString name = path.SubString( path.rfind(Separator) + 1, -1 );

        return name;
    }

    // return name with Separatorarator if directory
    wxString GetNameWithSep( wxString path )
    {
        bool isDir = path.EndsWith(Separator);
        if( isDir )
            path.RemoveLast();
        
        wxString name = GetName(path);
        name = isDir ? name + Separator : name;
        return name;
    }

}

};