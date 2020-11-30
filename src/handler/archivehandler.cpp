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

ArchiveHandler::ArchiveHandler( wxString path )
{
    this->Open( path );
}

void ArchiveHandler::Open( wxString path )
{
    this->filename = path;
}

bool ArchiveHandler::IsExist( int index )
{
    if ( this->fstream.empty() ) { return false; }
    return index >= 0 && index < int(this->fstream.size()) ;
}

void ArchiveHandler::Traverse()
{
    wxString path = this->filename;

    wxInputStream* stream;

    const wxArchiveClassFactory* factory;
    this->Find( path, factory, stream );
    this->Traverse( path, factory, stream );
}

void ArchiveHandler::Traverse( wxString path, const wxArchiveClassFactory* factory, wxInputStream* instream )
{
    // const wxArchiveClassFactory* fact = wxArchiveClassFactory::Find( path, wxSTREAM_FILEEXT );
    wxArchiveInputStream* stream = factory->NewStream( instream );
    wxArchiveEntry* entry;

    while ( (entry = stream->GetNextEntry()) != NULL )
    {
        if ( entry->IsDir() )
        {
            continue;
        }

        this->name.push_back( entry->GetName() );
    }
    this->name.Sort(wxCmpNaturalGeneric);

    stream = factory->NewStream( new wxFileInputStream(path) );
    while ( ( entry = stream->GetNextEntry()) )
    {
        for ( int i = 0; i < int(this->name.size()); i++ )
        {
            if ( entry->GetName() == this->name.Item(i) )
            {
                wxMemoryOutputStream file;
                stream->Read(file);
                this->fstream.push_back( new wxMemoryInputStream(file) );
                this->name.RemoveAt(i);
            }
        }
    }
    // delete stream;
    // delete entry;
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
    delete in;
    return result;
}

void ArchiveHandler::Clear()
{
    for ( auto& it : fstream )
    {
        it->~wxInputStream();
    }
    fstream.clear();
    this->filename = wxEmptyString;
    name.clear();
    this->factory = NULL;
}

ArchiveHandler::~ArchiveHandler()
{
    this->Clear();
}