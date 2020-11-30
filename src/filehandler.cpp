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

#include "filehandler.h"
#include <iostream>
#include <wx/stopwatch.h>

void FileHandler::Open( wxString path )
{
    wxStopWatch sw;
    wxInputStream* in = new wxFileInputStream(path);
    if ( wxImage::CanRead(path) )
    {
        this->type = 0;
        this->TraverseDir(path);
        std::cout << sw.Time() << std::endl;
        return;
    }

    const wxArchiveClassFactory* factory = wxArchiveClassFactory::Find(path, wxSTREAM_FILEEXT);
    this->filename = path;
    if ( factory )
    {
        this->type = 1;
        this->TraverseArchive(factory,in);
        std::cout << sw.Time() << std::endl;
        return;
    }

    // for archive with extension like .tar.gz
    const wxFilterClassFactory* fcf;
    fcf = wxFilterClassFactory::Find(path, wxSTREAM_FILEEXT);
    if ( fcf )
    {
        in = fcf->NewStream(in);
        path = fcf->PopExtension(path);
    }

    factory = wxArchiveClassFactory::Find(path,wxSTREAM_FILEEXT);
    if ( factory )
    {
        this->TraverseArchive(factory,in);
        return;
    }
}

bool FileHandler::IsExist( int index )
{
    return (index >= 0 && index < int(this->fstream.size()));
}

int FileHandler::Index( wxString path )
{
    if ( this->type == 0 )
    {
        return this->files.Index( path.AfterLast( wxFileName::GetPathSeparator() ) ); 
    }
    if ( this->type == 1 )
    {
        if ( path == this->filename )
        {
            return 0;
        }
    }
}

void FileHandler::TraverseArchive( const wxArchiveClassFactory* factory, wxInputStream* stream )
{
    wxArchiveInputStream* aistream = factory->NewStream(stream);
    wxArchiveEntry* entry;
    while ( (entry = aistream->GetNextEntry()) != NULL )
    {
        if ( entry->IsDir() )
        {
            continue;
        }

        this->files.push_back(entry->GetName());
    }
    this->files.Sort(wxCmpNaturalGeneric);

    aistream = factory->NewStream(stream);
    while ( ( entry = aistream->GetNextEntry()) != NULL )
    {
        for ( int i = 0; i < int(this->files.size()); i++ )
        {
            if ( entry->GetName() == this->files.Item(i) )
            {
                wxMemoryOutputStream file;
                aistream->Read(file);
                this->fstream.push_back( new wxMemoryInputStream(file) );
                this->files.RemoveAt(i);
            }
        }
    }
    delete aistream;
    delete entry;
}

void FileHandler::TraverseDir( wxString path  )
{
    this->dir.Open( wxPathOnly(path) );
    
    wxString filename;
    bool cont = this->dir.GetFirst(&filename, wxEmptyString, wxDIR_FILES);
    this->GetAllFiles(this->dir, cont, filename, this->files );
    this->files.Sort( wxCmpNaturalGeneric );
    for ( const auto& it : this->files )
    {
        this->fstream.push_back( new wxFileStream( this->dir.GetNameWithSep() + it ) );
    }
    cont = this->dir.GetFirst(&filename,wxEmptyString, wxDIR_DIRS );
    this->GetAllFiles(this->dir, cont, filename, this->directory );
    for ( const auto& it : this->directory )
    {
    }
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

void FileHandler::Clear()
{
    for ( auto& it : this->fstream )
    {
        delete it;
    }
    this->fstream.clear();
}