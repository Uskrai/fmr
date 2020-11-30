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

#ifndef HANDLER_DEFAULTHANDLER
#define HANDLER_DEFAULTHANDLER


// #include <wx/wfstream.h>

#include "handler/handler.h"
#include <wx/dir.h>
#include <wx/filename.h>

class wxDir;

class DefaultHandler
    : public Handler
{
    public:
        DefaultHandler(){}
        DefaultHandler( const wxString& path );
        ~DefaultHandler();
        void Open( const wxString& path );
        void Traverse( );

        int Index( const wxString& name );
        int IndexFilename( wxString path ) { return this->Index(  path.AfterLast( wxFileName::GetPathSeparator() )  ); }
        wxInputStream* Item( int index ) { return this->fstream.at(index); }
        int Size() { return this->files.size(); }

        bool IsExist( int index );

        void Clear();

        static bool CanHandle( wxString path ) { return true; }
        static void GetAllFiles( const wxString& path, wxVector<wxInputStream*>& stream );
        
    private:

        wxVector<wxInputStream*> fstream;
        int type;
        wxString filename;
        wxArrayString files;
        wxArrayString directory;
        wxDir dir;
        
        void GetAllFiles( wxDir& dir, bool& cont, wxString& filename, wxArrayString& array);

};

#endif