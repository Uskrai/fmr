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

        bool GetFirst( SStream &stream, DirGetFlags flags = kDirDefault, bool is_get_steam = false );
        bool GetNextStream( SStream &stream, bool is_get_stream = false );

        void Traverse( bool GetStream = false, DirGetFlags flags = kDirDefault );
        void TraverseStream();

        const std::shared_ptr<AbstractHandler> GetParent() const;
        std::shared_ptr<AbstractHandler> GetParent();

        const std::vector<struct SStream> &GetChild() const;
        std::vector<struct SStream> &GetChild();
        const wxString &GetName() const;

        wxString GetFromCurrent( int i ) const;
        wxString GetNext() const;
        wxString GetPrev() const;

        wxString ItemName( size_t idx );
        const struct SStream &Item( size_t index ) const;
        struct SStream &Item( size_t index );

        size_t Index( const wxString& name ) const;
        size_t Size() const;

        bool IsExist( size_t index ) const;
        bool IsOpened() const;

        void Reset();
        void Clear();
        void Close();

        bool CreateDirectories();
        bool CreateDirectories( const std::wstring &path );
        bool CreateDirectory( const std::wstring &directory_name, bool overwrite = false );
        bool CreateFiles( SStream stream, const std::wstring &filename, bool overwite = false );
        bool Remove( const std::wstring &filename, bool recursive = false );
        bool RemoveAll();
        bool RemoveAll( const std::wstring &path );
        bool CommitWrite();

        static bool CanHandle( wxString path ) { return true; }
    private:

        int type;
        wxString opened_name_;
        wxString m_filename;
        bool is_opened_ = false;
        wxDir opened_directory_;
        
        std::shared_ptr<DefaultHandler> parent_handler_ = NULL;

        std::vector<struct SStream> m_all, list_write_stream_;
        HandlerSortFirst sort_flag_ = kSortFileFirst;

        void OpenStream( const std::wstring &filename, SStream &stream, bool is_get_stream );

        void DoRemove( const SStream &stream );
        void DoCreateDirectory( const SStream &stream );
        void DoCreateFile( const SStream &stream );
};

};
#endif