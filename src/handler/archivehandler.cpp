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

#include "handler/archivehandler.h"
#include "handler/defaulthandler.h"
#include "base/path.h"
#include "base/compare.h"

ArchiveHandler::ArchiveHandler( const wxString& path )
{
    Open( path );
}

void ArchiveHandler::Open( const wxString& path )
{
    m_name = path;

    wxString parent = Path::GetParent(path);
    if ( parent != m_name )
    {
        m_parent = new DefaultHandler(parent);
        m_parentName = parent;
    }
}

size_t ArchiveHandler::Index( const wxString& name )
{
    if ( name == m_name ) return 0;
    return m_files.Index(name);
}

std::shared_ptr<wxInputStream> ArchiveHandler::Item( size_t index )
{
    auto stream = m_fstream.at(index);

    wxMemoryInputStream *result = new wxMemoryInputStream( *stream );
    return std::shared_ptr<wxInputStream>(result);
}

wxString ArchiveHandler::GetFromCurrent( int i )
{
    if ( GetParent() )
    {
        wxArrayString& names = GetParent()->GetChild();
        size_t idx = names.Index( Path::GetName( GetName() ));
        if ( idx != size_t(-1) && Vector::IsExist(names, idx + i ) )
            return GetParent()->GetName() + names.Item( idx + i );
    }

    return wxEmptyString;
}

wxString ArchiveHandler::GetNext() { return GetFromCurrent(1); }
wxString ArchiveHandler::GetPrev() { return GetFromCurrent(-1); }

void ArchiveHandler::Traverse( bool GetStream)
{
    wxString path = m_name;

    wxInputStream* instream;

    const wxArchiveClassFactory* factory;

    Find( path, factory, instream );

    wxArchiveInputStream* stream = factory->NewStream( *instream );
    wxArchiveEntry* entry;

    while ( (entry = stream->GetNextEntry()) != NULL )
    {
        if ( entry->IsDir() )
        {
            continue;
        }

        m_files.push_back( entry->GetName() );
    }
    m_files.Sort(Compare::Natural);

    delete stream;
    delete instream;

    if ( GetStream ) TraverseStream();
}

void ArchiveHandler::TraverseStream()
{
    wxInputStream *instream;
    const wxArchiveClassFactory* fct;
    Find( m_name, fct, instream );
    wxArchiveEntry *entry;
    wxArchiveInputStream *archivestream = fct->NewStream( *instream );

    wxArrayString files = m_files;
    while ( ( entry = archivestream->GetNextEntry()) )
    {
        for ( int i = 0; i < int(files.size()); i++ )
        {
            if ( entry->GetName() == files.Item(i) )
            {
                wxMemoryOutputStream *ostream = new wxMemoryOutputStream();
                ostream->Write( *archivestream );
                if ( ostream->GetSize() == 0 || ! ostream->IsOk())
                    ostream->Write( &DUMMY_BUFFER, sizeof(DUMMY_BUFFER) );
                
                m_fstream.push_back( std::shared_ptr<wxMemoryOutputStream>(ostream) );
                    
                files.RemoveAt(i);
            }
        }
    }

    archivestream->CloseEntry();
    delete archivestream;
    delete instream;
}

bool ArchiveHandler::Find( wxString& path, const wxArchiveClassFactory*& factory, wxInputStream*& in )
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

bool ArchiveHandler::CanHandle( wxString path )
{
    wxInputStream* in;
    const wxArchiveClassFactory* factory;
    bool result = ArchiveHandler::Find( path, factory, in );
    if ( result ) delete in;
    return result;
}

void ArchiveHandler::Clear()
{
    m_fstream.clear();
    m_name = wxEmptyString;
    m_files.clear();
    m_all.clear();
}

ArchiveHandler::~ArchiveHandler()
{
    Clear();
}