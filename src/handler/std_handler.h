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

#ifndef FMR_HANDLER_STD_HANDLER
#define FMR_HANDLER_STD_HANDLER


#include "handler/abstract_handler.h"
#include "handler/struct_stream.h"
#include "base/vector.h"

#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/mstream.h>

#include <memory>
#include <filesystem>


namespace fmr
{

class STDHandler
    : public AbstractHandler
{
    public:
        STDHandler(){};
        STDHandler( const wxString &path );
        ~STDHandler() {}

        const std::shared_ptr<AbstractHandler> GetParent() const;
        std::shared_ptr<AbstractHandler> GetParent();

        const std::vector<struct SStream> &GetChild() const;
        std::vector<struct SStream> &GetChild();

        const wxString &GetName() const;

        wxString GetFromCurrent( int step ) const;
        wxString GetNext() const;
        wxString GetPrev() const;

        void Open( const wxString &path );
        bool IsOpened() const;

        void Traverse( bool GetStream = false, DirGetFlags flags = kDirDefault );

        bool IsExist( size_t idx ) const;

        size_t Index( const wxString &name ) const;
        const struct SStream &Item( size_t idx ) const;
        struct SStream &Item( size_t idx );

        size_t Size() const;
        void Clear();
        void Reset();
        void Close();

        static bool CanHandle( wxString path );
        static bool StreamOpenable();
        bool IsStreamOpenable() const;

        bool OpenStream( SStream &stream );
        bool OpenStream( const std::wstring &name, SStream &stream, bool is_get_stream = false );
        bool GetFirst( SStream &stream, DirGetFlags flags = kDirDefault, bool is_get_stream = false );
        bool GetNextStream( SStream &stream, bool is_get_stream = false );

        bool CreateDirectory( const std::wstring directory_name, bool overwrite = false );
        bool CreateFiles( const std::wstring &file_name, SStream stream, bool overwrite = false );
        bool CreateDirectories();
        bool RemoveAll();
        bool Remove( const std::wstring &name, bool recursive );
        bool CommitWrite();
    private:
        void TraverseStream();

        std::filesystem::directory_iterator iterator_item_;
        DirGetFlags iterator_flags_;
        std::filesystem::directory_iterator iterator_;

        wxString name_, filename_;
        bool is_opened_;

        std::vector<struct SStream> list_stream_, list_write_stream_;
        std::shared_ptr<AbstractHandler> parent_ = NULL;
};

};

#endif
