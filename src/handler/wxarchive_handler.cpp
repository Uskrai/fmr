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

#include "handler/wxarchive_handler.h"
#include "handler/default_handler.h"

#include "base/compare.h"
#include <algorithm>

namespace fmr
{

WxArchiveHandler::WxArchiveHandler( const wxString& path )
{
    Open( path );
}

void WxArchiveHandler::Open( const wxString& path )
{
    m_name = path;

    wxString parent = Path::GetParent(path);
    if ( parent != m_name )
    {
        m_parent = new DefaultHandler(parent);
        m_parentName = parent;
    }
}

size_t WxArchiveHandler::Index( const wxString& name )
{
    if ( name == m_name ) return 0;

    size_t idx = 0;
    while( Vector::IsExist( m_all, idx ) )
        if ( m_all.at(idx).GetName() == name )
            return idx;
            
    return -1;
}

SStream &WxArchiveHandler::Item( size_t index )
{
    // auto stream = m_fstream.at(index);

    // wxMemoryInputStream *result = new wxMemoryInputStream( *stream );
    // return std::shared_ptr<wxInputStream>(result);
    return GetChild().at(index);
}

wxString WxArchiveHandler::GetFromCurrent( int i )
{
    if ( GetParent() )
    {
        auto &list_stream = GetParent()->GetChild();
        size_t idx = GetParent()->Index( Path::GetName( GetName() ));
        if ( idx != size_t(-1) && Vector::IsExist( GetParent()->GetChild(), idx + i ) )
            return list_stream.at( idx + i ).GetName();
    }

    return wxEmptyString;
}

wxString WxArchiveHandler::GetNext() { return GetFromCurrent(1); }
wxString WxArchiveHandler::GetPrev() { return GetFromCurrent(-1); }

void WxArchiveHandler::Traverse( bool GetStream)
{
    wxString path = m_name;

    wxInputStream* instream;

    const wxArchiveClassFactory* factory;

    Find( path, factory, instream );

    wxArchiveInputStream* stream = factory->NewStream( *instream );
    wxArchiveEntry* entry;

    while ( (entry = stream->GetNextEntry()) != NULL )
    {
        m_all.push_back( SStream() );
        m_all.back().SetName( entry->GetName() );
    }

    std::sort( m_all.begin(), m_all.end(), Compare::NaturalSortable );

    delete stream;
    delete instream;

    if ( GetStream ) TraverseStream();
}

void WxArchiveHandler::TraverseStream()
{
    wxInputStream *instream;
    const wxArchiveClassFactory* fct;
    Find( m_name, fct, instream );
    wxArchiveEntry *entry;
    wxArchiveInputStream *archivestream = fct->NewStream( *instream );

    while ( ( entry = archivestream->GetNextEntry()) )
        for ( int i = 0; i < int(m_all.size()); i++ )
            if ( entry->GetName() == m_all.at(i).GetName() )
                m_all.at(i).Open( archivestream );

    archivestream->CloseEntry();
    delete archivestream;
    delete instream;
}

bool WxArchiveHandler::Find( wxString& path, const wxArchiveClassFactory*& factory, wxInputStream*& in )
{

    factory = wxArchiveClassFactory::Find( path, wxSTREAM_FILEEXT );
    if ( factory ) 
    {
        if ( !in ) delete in;
        in = new wxFileInputStream(path);
        return true;
    }

    wxString filename = path;

    const wxFilterClassFactory* fcf;
    fcf = wxFilterClassFactory::Find( path, wxSTREAM_FILEEXT );

    if ( fcf )
    {
        if ( !in ) delete in;
        in = fcf->NewStream( new wxFileInputStream(filename));
        path = fcf->PopExtension(path);

        factory = wxArchiveClassFactory::Find( path, wxSTREAM_FILEEXT );

        if ( factory )
        {
            return true;
        }
    }

    return false;
}

bool WxArchiveHandler::CanHandle( wxString path )
{
    wxInputStream* in;
    const wxArchiveClassFactory* factory;
    bool result = WxArchiveHandler::Find( path, factory, in );
    if ( result ) delete in;
    return result;
}

void WxArchiveHandler::Clear()
{
    m_fstream.clear();
    m_name = wxEmptyString;
    m_files.clear();
    m_all.clear();
}

WxArchiveHandler::~WxArchiveHandler()
{
    Clear();
}

}; // namespace fmr