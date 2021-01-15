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
#include <filesystem>

namespace fs = std::filesystem;

namespace fmr
{

namespace Path
{
    // wxString GetFullPath( wxString path )
    // {
    //     wxFileName name(path);
    //     if ( path.StartsWith('.') )
    //         path = wxFileName::GetCwd() + Separator + path.AfterFirst( Separator );
    
    //     RemoveDirSep(path);
    //     if ( name.DirExists(path) )
    //         name.AssignDir(path);
    //     else if ( name.FileExists(path) )
    //         name.Assign(path);

    //     return name.GetFullPath();
    // }

    // wxString GetDirName( wxString path )
    // {
    //     path = GetFullPath(path);
    //     if ( path.EndsWith(Separator) )
    //         return path;

    //     size_t idx = path.rfind( Separator );
    //     return path.SubString( 0, idx );
    // }

    // // return parent's path
    // wxString GetParent( const wxString& path )
    // {
    //     wxString name(GetFullPath(path));

    //     RemoveDirSep(name);

    //     if ( name.empty() )
    //         return wxFileName::GetCwd();

    //     return name.SubString(0,name.rfind(Separator));
    // }

    // // strip last Separatorarator
    // void RemoveDirSep( wxString& path )
    // {
    //     if ( path.EndsWith(Separator) )
    //         path.RemoveLast();
    // }

    // void RemoveDirSep( String &string )
    // {
    //     if ( string.back() == Separator )
    //         string.pop_back();
    // }

    // // return Name without Separatorarator
    // wxString GetName( wxString path )
    // {
    //     RemoveDirSep(path);
        
    //     wxString name = path.SubString( path.rfind(Separator) + 1, -1 );

    //     return name;
    // }

    // // return name with Separatorarator if directory
    // wxString GetNameWithSep( wxString path )
    // {
    //     bool isDir = path.EndsWith(Separator);
    //     if( isDir )
    //         path.RemoveLast();
        
    //     wxString name = GetName(path);
    //     name = isDir ? name + Separator : name;
    //     return name;
    // }

    std::string MakeString( const fs::path  &path )
        { return path.u8string(); }

    std::string MakeString( const wxString &path )
            { return std::string( path.ToUTF8() ); }

    std::string GetSeparator()
        { return std::string( 1, Separator ); }

    std::string GetParent( std::string path )
    {
        if ( !IsRoot( path ) )
            RemoveDirSep( path );

        fs::path temp( path );
        fs::path temp_parent = temp.parent_path();

        return GetDirName( MakeString( temp_parent ) );
    }

    std::string GetName( std::string path )
    {
        RemoveDirSep( path );


        size_t idx = path.rfind( Separator );
        if ( idx != std::string::npos )
            path = path.substr( idx );
        return path;
    }

    std::string GetRootPath( const std::string &path )
        { return MakeString( fs::path(path).root_path() ); }

    std::string GetDirName( const std::string &path )
    {
        fs::path temp(path);

        if ( fs::is_directory( temp ) )
            return MakeString( temp );

        temp.remove_filename();

        if ( temp.empty() )
            return MakeString( fs::current_path() );

        return MakeString( temp );
    }

    void RemoveDirSep( std::string &path )
    {
        if ( !path.empty() && path.back() == Separator )
            path.pop_back();
    }

    bool HasRootPath( const std::string &path )
        { return fs::path(path).has_root_path(); }

    bool IsRoot( const std::string &path )
        { return GetRootPath( path ) == path; }

    bool IsChild( const std::string &parent, std::string target )
    {
        std::string parent_root = GetRootPath( parent );
        std::string target_root = GetRootPath( target );

        if ( !HasRootPath( target ) )
            return true;

        if ( target_root != parent_root )
            return false;

        if ( parent_root == parent && parent_root == target_root )
            return true;

        while( target != target_root )
        {
            target = GetParent( target );
            if ( target == parent )
                return true;
        }

        return false;
    }

    bool IsAbsolute( const std::string &path )
        { return fs::path(path).is_absolute(); }

    bool IsRelative( const std::string &path )
        { return fs::path(path).is_relative(); }

    std::string Append( const std::string &parent, const std::string &target )
    {
        fs::path path( parent );
        return MakeString( path / target );
    }

    std::string MakeRelative( const std::string &parent, const std::string &target )
    {
        return MakeString( fs::relative( target, parent ) );
    }

    std::string MakeAbsolute( const std::string &path )
    { return MakeString( fs::absolute( path ) ); }

    std::string MakeDirectory( const std::string &path )
    {
        if ( fs::is_directory( path ) )
            return path;

        return Append( path, GetSeparator() );
    }
}

};