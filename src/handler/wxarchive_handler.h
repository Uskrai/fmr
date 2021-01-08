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
#include "handler/default_handler.h"
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
        WxArchiveHandler( const std::string& path );
        void Open( const std::string& path );

        const std::string &GetName() const;

        const std::shared_ptr<AbstractHandler> GetParent() const;
        std::shared_ptr<AbstractHandler> GetParent();

        const std::vector<SStream> &GetChild() const;
        std::vector<SStream> &GetChild();

        std::string GetFromCurrent( int step ) const;
        std::string GetNext() const;
        std::string GetPrev() const;

        bool IsExist( size_t index ) const;
        bool IsOpened() const;

        bool GetFirst( SStream &stream, DirGetFlags flags = kDirDefault, bool is_get_stream = false );
        bool GetNextStream( SStream &stream, bool is_get_stream = false );
        bool GetStream( SStream &stream );

        void Traverse( bool GetStream = false, DirGetFlags flags = kDirDefault );
        void TraverseStream();
        size_t Index( const std::string& name ) const;
        size_t Size() const;

        const SStream &Item( size_t index ) const;
        SStream &Item( size_t index );

        void Reset();
        void Clear();
        void Close();

        ~WxArchiveHandler();


        bool CreateDirectories();
        bool CreateDirectories( const std::string &path );
        bool CreateDirectory( const std::string &dirname, bool overwrite = false );
        bool CreateFiles( SStream stream, const std::string &name, bool overwrite = false );
        bool Remove( const std::string &name, bool recursive = false );
        bool RemoveAll();
        bool RemoveAll( const std::string &path );
        bool CommitWrite();

        static bool CanHandle( std::string path );
    private:
        static bool Find( std::string path, const wxArchiveClassFactory*& factory );

        std::shared_ptr<DefaultHandler> m_parent;

        std::string m_name;
        std::string m_parentName;

        bool is_opened_ = false;


        DirGetFlags iterator_flags_ = kDirNone;
        wxArchiveInputStream *iterator_item_ = NULL;

        std::vector<SStream> &GetWriteList();

        std::vector<SStream> m_all, list_write_stream_;
};

}; // namespace fmr
#endif