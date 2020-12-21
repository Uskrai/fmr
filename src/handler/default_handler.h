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

#include "handler/abstract_handler.h"
#include "base/vector.h"

#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/mstream.h>

#include <memory>

class wxDir;

namespace fmr
{

class DefaultHandler
    : public AbstractHandler
{
    public:
        DefaultHandler(){}
        DefaultHandler( const wxString& path );
        ~DefaultHandler();
        void Open( const wxString& path );
        void Traverse( bool GetStream = false );
        void TraverseStream();

        AbstractHandler *GetParent() { return m_parent; }
        std::vector<struct SStream> &GetChild() { return m_all; }
        wxString GetName() { return m_name; }

        wxString GetFromCurrent( int i );
        wxString GetNext();
        wxString GetPrev();

        wxString ItemName( size_t idx ) { return m_files.Item(idx); }
        struct SStream &Item( size_t index );

        size_t Index( const wxString& name );
        size_t IndexFilename( wxString path );
        size_t Size() { return m_all.size(); }

        bool IsExist( size_t index )
                { return Vector::IsExist(m_all,index); }

        void Clear();
        void Close();

        static bool CanHandle( wxString path ) { return true; }
    private:

        int type;
        wxString m_name;
        wxString m_filename;
        wxString m_parentName;
        
        AbstractHandler* m_parent;

        std::vector<struct SStream> m_all;
        HandlerSortFirst sort_flag_ = kSortFileFirst;
        wxArrayString m_files;
        wxArrayString m_directory;
        wxDir dir;
        
        void GetAllFiles( std::vector<struct SStream> &vec_stream, int dir_get_flag );

};

};
#endif