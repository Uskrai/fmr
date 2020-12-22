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
#ifndef FMR_HANDLER_ABSTRACT_HANDLER
#define FMR_HANDLER_ABSTRACT_HANDLER

#include "handler/struct_stream.h"

#include <memory>
#include <vector>
#include <wx/stream.h>

namespace fmr
{

enum HandlerSortFirst
{
    kSortFileFirst,
    kSortDirFirst,
    kSortAll
};

class AbstractHandler 
{
    public:     
        AbstractHandler() {};
        AbstractHandler( const wxString& path ) {};
        virtual ~AbstractHandler() {};

        // return handler of Parent's Directory
        virtual const AbstractHandler *GetParent() const = 0; 
        virtual AbstractHandler *GetParent() = 0;

        // return all directory and files in current dir
        virtual const std::vector<struct SStream> &GetChild() const = 0;
        virtual std::vector<struct SStream> &GetChild() = 0;

        // return opened path
        virtual const wxString &GetName() const = 0;
        virtual wxString GetFromCurrent( int step ) const = 0;
        // return next or prev folder / file
        virtual wxString GetNext() const = 0;
        virtual wxString GetPrev() const = 0;

        virtual void Open( const wxString& path) = 0;
        // enumerate current opened file or dir
        virtual void Traverse( bool GetStream = false ) = 0 ;

        virtual bool IsExist( size_t index ) const = 0;

        // search from filename
        virtual size_t Index( const wxString& name ) const = 0;
        // return files stream;
        virtual const struct SStream &Item( size_t index ) const = 0;
        virtual struct SStream &Item( size_t index ) = 0;

        virtual size_t Size() const = 0;
        virtual void Clear() = 0;
};

}; // namespace fmr

#endif