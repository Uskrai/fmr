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

#ifndef TRAVERSER
#define TRAVERSER

#include <wx/dir.h>
#include <wx/filename.h>

class Traverser :
    public wxDirTraverser
{
    public:
        void Open( wxString path );
        int Index( wxString name ) { return this->files.Index( name.AfterLast( wxFileName::GetPathSeparator() ) ); }
        int IndexFilename( wxString path ) { return this->Index(  path.AfterLast( wxFileName::GetPathSeparator() )  ); }
        wxString Item( int index ) { return this->dir->GetNameWithSep() + this->files.Item(index); }
        int Size() { return this->files.size(); }
        bool IsExist(int index ) { return index > 0 && index < int(this->files.size()) ;}
    
    private:
        wxDirTraverseResult OnDir( const wxString& name );
        wxDirTraverseResult OnFile( const wxString& name );
        wxDirTraverseResult OnOpenError( const wxString& name );
       
        wxArrayString files;
        wxArrayString directory;
        wxDir* dir = new wxDir();

};

#endif