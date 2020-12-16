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

#include "handler/default_handler.h"
#include <wx/wfstream.h>

#include "base/path.h"
#include "base/compare.h"
#include <algorithm>
#include <wx/wxcrtvararg.h>

namespace fmr
{


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

SStream &DefaultHandler::Item( size_t idx ) 
{ 
    return m_all.at(idx);
}

size_t DefaultHandler::IndexFilename( wxString path )
{
    return 0;
}

wxString DefaultHandler::GetFromCurrent( int i )
{
    if ( GetParent() )
    {
        std::vector<SStream> &list_stream = GetParent()->GetChild();
        size_t idx = GetParent()->Index( GetName() );
        if ( idx != size_t(-1) && Vector::IsExist(list_stream, idx + i ) )
            return list_stream.at(idx).GetName();
    }
    return wxEmptyString;
}

wxString DefaultHandler::GetNext() { return GetFromCurrent( 1 ); }

wxString DefaultHandler::GetPrev() { return GetFromCurrent( -1 ); }

size_t DefaultHandler::Index( const wxString& path )
{
    size_t idx = 0;
    while( Vector::IsExist( m_all, idx) )
    {
        if ( m_all.at(idx).GetName() == path )
            return idx;
        idx++;
    }
    return idx;    
}

void DefaultHandler::Traverse( bool GetStream )
{
    dir.Open( wxPathOnly( m_name ) );
    
    if ( ! dir.IsOpened() ) return;

    wxString filename;
    bool entry = dir.GetFirst(&filename, wxEmptyString );
    GetAllFiles( dir, entry, filename, m_all );

    std::sort( m_all.begin(), m_all.end(), Compare::NaturalSortable );

    dir.Close();
    if ( GetStream ) TraverseStream();
}

void DefaultHandler::TraverseStream()
{
    for ( auto &it : m_all )
        it.Open( it.GetName() );
}

void DefaultHandler::GetAllFiles( wxDir &dir, bool &cont, wxString &filename, std::vector<SStream> &list_stream, bool isGetStream )
{
    while ( cont )
    {
        wxFileName name(filename);
        m_all.push_back( SStream() );
        m_all.back().SetName( dir.GetNameWithSep() + filename );

        cont = dir.GetNext(&filename);
    }
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

void DefaultHandler::Clear()
{
    m_all.clear();
}

void DefaultHandler::Close()
{
    Clear();
    m_name = wxEmptyString;
}

DefaultHandler::~DefaultHandler()
{
    Close();
}

}; // namespace fmr