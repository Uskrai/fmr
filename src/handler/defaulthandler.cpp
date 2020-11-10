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
#include "base/compare.h"
#include <iostream>

DefaultHandler::DefaultHandler( const wxString& path )
{
    this->Open(path);
}
void DefaultHandler::Open( const wxString& path )
{
    m_name = Path::GetDirName(path);

    wxString parent = Path::GetParent(m_name);
    if ( parent != m_name )
    {
        m_parent = new DefaultHandler( parent );
        m_parentName = parent;
    }    
}

size_t DefaultHandler::IndexFilename( wxString path )
{
    return 0;
}

wxString DefaultHandler::GetNextPrev( int i )
{
    if ( GetParent() )
    {
        wxString parent = GetParent()->GetName();
        wxArrayString &name = GetParent()->GetChild();
        size_t idx = name.Index( Path::GetName(m_name) );
        if ( idx != size_t(-1) && Vector::IsExist(name, idx + i ) )
        {
            return GetParent()->GetName() + name.Item( idx + i );
        }
    }
    return wxEmptyString;
}

wxString DefaultHandler::GetNext() { return GetNextPrev( 1 ); }

wxString DefaultHandler::GetPrev() { return GetNextPrev( -1 ); }

size_t DefaultHandler::Index( const wxString& path )
{
    size_t idx = m_files.Index( Path::GetName(path) );
    
    idx = ( idx != size_t(-1) ) ? idx : 0;
    return idx;
}

void DefaultHandler::Traverse()
{
    dir.Open( wxPathOnly( m_name ) );
    
    wxString filename;
    bool entry = dir.GetFirst(&filename, wxEmptyString, wxDIR_FILES);
    GetAllFiles( dir, entry, filename, m_files );
    m_files.Sort( Compare::Natural );
    for ( const auto& it : m_files )
    {
        m_fstream.push_back( new wxFileInputStream( dir.GetNameWithSep() + it ) );
    }
    entry = dir.GetFirst(&filename,wxEmptyString, wxDIR_DIRS );
    GetAllFiles( dir, entry, filename, m_directory );
    m_directory.Sort( Compare::Natural );
    
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
    m_name = wxEmptyString;
}

DefaultHandler::~DefaultHandler()
{
    this->Clear();
}