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
#ifndef FMR_HANDLER_WX_ARCHIVE_HANDLER
#define FMR_HANDLER_WX_ARCHIVE_HANDLER

#include "handler/abstract_handler.h"
#include "handler/struct_stream.h"
#include "base/vector.h"
#include "base/path.h"

#include <wx/archive.h>
#include <wx/wfstream.h>
#include <wx/mstream.h>

namespace fmr
{

class WxArchiveHandler 
    : public AbstractHandler
{
    public:
        WxArchiveHandler() {}
        WxArchiveHandler( const wxString& path );
        void Open( const wxString& path );

        wxString GetName() { return m_name; }
        AbstractHandler *GetParent() { return m_parent; }
        wxString GetParentName() { return m_parentName; }
        std::vector<SStream> &GetChild() { return m_all; }

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
        SStream &Item( size_t index );
        void Clear();

        ~WxArchiveHandler();

        static bool CanHandle( wxString path );
    private:
        static bool Find( wxString& path, const wxArchiveClassFactory*& factory, wxInputStream*& in );

        AbstractHandler *m_parent;

        wxString m_name;
        wxString m_parentName;

        wxArrayString m_files;
        std::vector<SStream> m_all;

        wxVector<std::shared_ptr<wxMemoryOutputStream>> m_fstream;
};

}; // namespace fmr
#endif