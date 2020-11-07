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

#include "handler/defaulthandler.h"
#include <wx/wfstream.h>
#include "base/path.h"
#include <iostream>

DefaultHandler::DefaultHandler( const wxString& path )
{
    this->Open(path);
}
void DefaultHandler::Open( const wxString& path )
{
    m_filename = Path::GetFullPath(path);

    wxString parent = Path::GetParent(path);
    if ( parent != m_filename )
    {
        m_parent = new DefaultHandler( parent );
        m_parentName = parent;
    }    
}

bool DefaultHandler::IsExist( int index )
{
    return (index >= 0 && index < int(m_fstream.size()));
}

wxString DefaultHandler::GetNextPrev( int i )
{
    if ( GetParent() )
    {
        
    }
    return wxEmptyString;
}

wxString DefaultHandler::GetNext() { return GetNextPrev( 1 ); }

wxString DefaultHandler::GetPrev() { return GetNextPrev( -1 ); }

int DefaultHandler::Index( const wxString& path )
{
    if ( m_files.size() > 0 )
    {
        return m_files.Index( path.AfterLast( wxFileName::GetPathSeparator() ) ); 
    }
    return -1;
}

void DefaultHandler::Traverse()
{
    dir.Open( wxPathOnly( m_filename ) );
    
    wxString filename;
    bool entry = dir.GetFirst(&filename, wxEmptyString, wxDIR_FILES);
    GetAllFiles( dir, entry, filename, m_files );
    m_files.Sort( wxCmpNaturalGeneric );
    for ( const auto& it : m_files )
    {
        m_fstream.push_back( new wxFileInputStream( dir.GetNameWithSep() + it ) );
    }
    entry = dir.GetFirst(&filename,wxEmptyString, wxDIR_DIRS );
    GetAllFiles( dir, entry, filename, m_directory );
    m_directory.Sort( wxCmpNaturalGeneric );
    
    m_all = m_directory;
    for ( const auto& it : m_files )
        m_all.push_back(it);
}

void DefaultHandler::GetAllFiles( wxDir& dir, bool& cont, wxString& filename, wxArrayString& array  )
{
    while ( cont )
    {
        wxFileName name(filename);
        if ( name.IsDir() )
        {
            array.push_back( name.GetFullPath() );
        }
        else {
            array.push_back( name.GetFullName() );
        }
        cont = dir.GetNext(&filename);
    }
}

void DefaultHandler::GetAllFiles( const wxString& path, wxVector<wxInputStream*>& stream )
{
    wxArrayString arr;
    wxDir::GetAllFiles( wxPathOnly(path), &arr );
    for ( const auto& it : arr )
    {
        stream.push_back( new wxFileStream(it) );
    }
}

void DefaultHandler::Clear()
{
    m_fstream.clear();
    m_files.clear();
    m_directory.clear();
    m_filename = wxEmptyString;
}

DefaultHandler::~DefaultHandler()
{
    this->Clear();
}