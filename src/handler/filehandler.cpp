/*
 * 
 * Copyright (c) 2020 Uskrai
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 */

#include "handler/filehandler.h"
#include <wx/wfstream.h>

FileHandler::FileHandler( const wxString& path )
{
    this->Open(path);
}
void FileHandler::Open( const wxString& path )
{
    this->filename = path;
}

bool FileHandler::IsExist( int index )
{
    return (index >= 0 && index < int(this->fstream.size()));
}

int FileHandler::Index( const wxString& path )
{
    if ( this->files.size() > 0 )
    {
        return this->files.Index( path.AfterLast( wxFileName::GetPathSeparator() ) ); 
    }
    return -1;
}

void FileHandler::Traverse()
{
    this->dir.Open( wxPathOnly( this->filename ) );
    
    wxString filename;
    bool entry = this->dir.GetFirst(&filename, wxEmptyString, wxDIR_FILES);
    this->GetAllFiles(this->dir, entry, filename, this->files );
    this->files.Sort( wxCmpNaturalGeneric );
    for ( const auto& it : this->files )
    {
        this->fstream.push_back( new wxFileInputStream( this->dir.GetNameWithSep() + it ) );
    }
    entry = this->dir.GetFirst(&filename,wxEmptyString, wxDIR_DIRS );
    this->GetAllFiles(this->dir, entry, filename, this->directory );
}

void FileHandler::GetAllFiles( wxDir& dir, bool& cont, wxString& filename, wxArrayString& array  )
{
    while ( cont )
    {
        wxFileName name(filename);
        if ( name.IsDir() )
        {
            array.push_back( name.GetFullPath().AfterLast( wxFileName::GetPathSeparator() ));
        }
        else {
            array.push_back(name.GetFullName());
        }
        cont = dir.GetNext(&filename);
    }
}

void FileHandler::GetAllFiles( const wxString& path, wxVector<wxInputStream*>& stream )
{
    wxArrayString arr;
    wxDir::GetAllFiles( wxPathOnly(path), &arr );
    for ( const auto& it : arr )
    {
        stream.push_back( new wxFileInputStream(it) );
    }
}

void FileHandler::Clear()
{
    this->fstream.clear();
    this->files.clear();
    this->directory.clear();
    this->dir.~wxDir();
    this->filename = wxEmptyString;
}

FileHandler::~FileHandler()
{
    this->Clear();
}