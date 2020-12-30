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

enum DirFlags
{
    DIR_FOLLOWSYMLINK   = 0x1,
    DIR_SKIPDENIED      = 0x2,
    DIR_DIRS            = 0x4,
    DIR_FILES           = 0x8,
    DIR_DEFAULT         = DIR_FILES | DIR_DIRS | DIR_FOLLOWSYMLINK | DIR_SKIPDENIED
};

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

        void Traverse( bool GetStream = false );

        bool IsExist( size_t idx ) const;

        size_t Index( const wxString &name ) const;
        const struct SStream &Item( size_t idx ) const;
        struct SStream &Item( size_t idx );

        size_t Size() const;
        void Clear();
        void Close();

        static bool CanHandle( wxString path );

        bool GetFirst( SStream &stream, int flags = DIR_DEFAULT, bool is_get_stream = false );
        bool GetNextFile( SStream &stream, bool is_get_stream = false );
    private:
        void TraverseStream();

        std::filesystem::directory_iterator iterator_item_;
        int iterator_flags_;
        std::filesystem::directory_iterator iterator_;

        wxString name_, filename_;

        std::vector<struct SStream> list_stream_;
        std::shared_ptr<AbstractHandler> parent_ = NULL;
};

};

#endif
