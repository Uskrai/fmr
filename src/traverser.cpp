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

#include "traverser.h"

void Traverser::Open( wxString path )
{
    this->dir->Open( wxPathOnly(path) );
    this->dir->Traverse(*(this) );
    this->files.Sort( wxCmpNaturalGeneric );
    for ( const auto& it : this->files )
    {
        wxFileStream* strm = new wxFileStream( this->dir->GetNameWithSep() + it);
        this->fstream.push_back( strm );
    }
}

wxDirTraverseResult Traverser::OnDir( const wxString& name )
{
    wxString filename = name;
    this->files.push_back( name.AfterLast( wxFileName::GetPathSeparator() ) );
    return wxDIR_IGNORE;
}

wxDirTraverseResult Traverser::OnFile( const wxString& name )
{
    this->files.push_back( name.AfterLast( wxFileName::GetPathSeparator() ) );
    return wxDIR_CONTINUE;
}

wxDirTraverseResult Traverser::OnOpenError( const wxString& name )
{
    return wxDIR_CONTINUE;
}