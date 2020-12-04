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
#include "base/vector.h"
#include "base/path.h"

class ArchiveHandler 
    : public Handler
{
    public:
        ArchiveHandler() {}
        ArchiveHandler( const wxString& path );
        void Open( const wxString& path );

        wxString GetName() { return m_name; }
        Handler* GetParent() { return m_parent; }
        wxString GetParentName() { return m_parentName; }
        wxArrayString& GetChild() { return m_all; }

        wxString GetFromCurrent( int i );
        wxString GetNext();
        wxString GetPrev();

        bool IsExist( size_t index ) 
            { return Vector::IsExist(m_fstream,index); }
        
        void Traverse( bool GetStream = false );
        void TraverseStream();
        size_t Index( const wxString& name );
        size_t Size() { return m_fstream.size();} ;

        wxString ItemName( size_t idx ) { return m_files.Item(idx); }
        wxInputStream* Item( size_t index ) { return m_fstream.at(index); }
        void Clear();

        ~ArchiveHandler();

        static bool CanHandle( wxString path );
    private:
        static bool Find( wxString& path, const wxArchiveClassFactory*& factory, wxInputStream*& in );

        Handler* m_parent;

        wxString m_name;
        wxString m_parentName;

        wxArrayString m_files;
        wxArrayString m_all;

        wxVector<wxInputStream*> m_fstream;
};

#endif