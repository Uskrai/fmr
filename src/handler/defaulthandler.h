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
#include "base/vector.h"

#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/mstream.h>

#include <memory>

class wxDir;

class DefaultHandler
    : public Handler
{
    public:
        DefaultHandler(){}
        DefaultHandler( const wxString& path );
        ~DefaultHandler();
        void Open( const wxString& path );
        void Traverse( bool GetStream = false );
        void TraverseStream();

        Handler* GetParent() { return m_parent; }
        wxArrayString& GetChild() { return m_all; }
        wxString GetName() { return m_name; }

        wxString GetFromCurrent( int i );
        wxString GetNext();
        wxString GetPrev();

        wxString ItemName( size_t idx ) { return m_files.Item(idx); }
        wxInputStream* Item( size_t index );

        size_t Index( const wxString& name );
        size_t IndexFilename( wxString path );
        size_t Size() { return m_files.size(); }

        bool IsExist( size_t index )
                { return Vector::IsExist(m_fstream,index); }

        void Clear();
        void Close();

        static bool CanHandle( wxString path ) { return true; }
    private:

        wxVector<std::shared_ptr<wxMemoryInputStream>> m_fstream;
        int type;
        wxString m_name;
        wxString m_filename;
        wxString m_parentName;
        
        Handler* m_parent;

        wxArrayString m_all;
        wxArrayString m_files;
        wxArrayString m_directory;
        wxDir dir;
        
        void GetAllFiles( wxDir& dir, bool& cont, wxString& filename, wxArrayString& array);

};

#endif