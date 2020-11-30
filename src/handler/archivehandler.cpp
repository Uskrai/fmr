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

ArchiveHandler::ArchiveHandler( const wxString& path )
{
    Open( path );
}

void ArchiveHandler::Open( const wxString& path )
{
    m_filename = path;

    wxString parent = Path::GetParent(path);
    if ( parent != m_filename )
    {
        m_parent = new DefaultHandler(parent);
        m_parentName = parent;
    }
}

wxString ArchiveHandler::GetNext() { return wxString(); }
wxString ArchiveHandler::GetPrev() { return wxString(); }

void ArchiveHandler::Traverse()
{
    wxString path = m_filename;

    wxInputStream* instream;

    const wxArchiveClassFactory* factory;

    Find( path, factory, instream );

    wxArchiveInputStream* stream = factory->NewStream( instream );
    wxArchiveEntry* entry;

    while ( (entry = stream->GetNextEntry()) != NULL )
    {
        if ( entry->IsDir() )
        {
            continue;
        }

        m_files.push_back( entry->GetName() );
    }
    m_files.Sort(wxCmpNaturalGeneric);

    stream = factory->NewStream( new wxFileInputStream(path) );
    while ( ( entry = stream->GetNextEntry()) )
    {
        for ( int i = 0; i < int(m_files.size()); i++ )
        {
            if ( entry->GetName() == m_files.Item(i) )
            {
                wxMemoryOutputStream file;
                stream->Read(file);
                m_fstream.push_back( new wxMemoryInputStream(file) );
                m_files.RemoveAt(i);
            }
        }
    }
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
    for ( auto& it : m_fstream )
    {
        it->~wxInputStream();
    }
    m_fstream.clear();
    m_filename = wxEmptyString;
    m_files.clear();
}

ArchiveHandler::~ArchiveHandler()
{
    Clear();
}