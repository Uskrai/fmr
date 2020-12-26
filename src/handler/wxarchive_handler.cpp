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
        m_parent = std::shared_ptr<AbstractHandler>(
            new DefaultHandler(parent)
        );
        m_parentName = parent;
    }
}

size_t WxArchiveHandler::Index( const wxString& name ) const
{
    if ( name == m_name ) return 0;

    size_t idx = 0;
    for ( const auto &it : GetChild() )
    {
        if ( it.GetName() == name )
            return idx;
        idx++;
    }

    return -1;
}

const wxString &WxArchiveHandler::GetName() const
    { return m_name; }

const std::shared_ptr<AbstractHandler> WxArchiveHandler::GetParent() const
    { return m_parent; }

std::shared_ptr<AbstractHandler> WxArchiveHandler::GetParent()
    { return m_parent; }

const std::vector<SStream> &WxArchiveHandler::GetChild() const
    { return m_all; }

std::vector<SStream> &WxArchiveHandler::GetChild()
    { return m_all; }

const SStream &WxArchiveHandler::Item( size_t index ) const
    { return GetChild().at(index); }

SStream &WxArchiveHandler::Item( size_t index )
    { return GetChild().at(index); }

size_t WxArchiveHandler::Size() const
    { return GetChild().size(); }

bool WxArchiveHandler::IsExist( size_t index ) const
    { return Vector::IsExist( GetChild(), index  ); }

wxString WxArchiveHandler::GetNext() const
    { return GetFromCurrent(1); }

wxString WxArchiveHandler::GetPrev() const
    { return GetFromCurrent(-1); }

wxString WxArchiveHandler::GetFromCurrent( int i ) const
{
    if ( GetParent() )
    {
        auto &list_stream = GetParent()->GetChild();
        size_t idx = GetParent()->Index( GetName() );
        if ( idx != size_t(-1) && Vector::IsExist( list_stream, idx + i ) )
            return list_stream.at( idx + i ).GetName();
    }

    return wxEmptyString;
}

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
        m_all.back().SetHandlerPath( GetName() );
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
    m_name = wxEmptyString;
    GetChild().clear();
}

WxArchiveHandler::~WxArchiveHandler()
{
    Clear();
}

}; // namespace fmr