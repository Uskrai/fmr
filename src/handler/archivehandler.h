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
#ifndef HANDLER_ARCHIVEHANDLER
#define HANDLER_ARCHIVEHANDLER


#include "handler/handler.h"

#include <wx/archive.h>
#include <wx/wfstream.h>
#include <wx/mstream.h>

class ArchiveHandler 
    : public Handler
{
    public:
        ArchiveHandler() {}
        ArchiveHandler( const wxString& path );
        void Open( const wxString& path );

        bool IsExist( int index );
        
        void Traverse();
        int Index( const wxString& name ) { return 0; } ;
        int Size() { return m_fstream.size();} ;
        wxInputStream* Item( int index ) { return m_fstream.at(index); };
        void Clear();

        ~ArchiveHandler();

        static bool CanHandle( wxString path );
    private:
        static bool Find( wxString& path, const wxArchiveClassFactory*& factory, wxInputStream*& in );

        wxString m_filename;
        wxArrayString m_files;
        wxVector<wxInputStream*> m_fstream;
};

#endif