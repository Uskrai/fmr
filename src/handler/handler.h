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
#ifndef HANDLER_HANDLER
#define HANDLER_HANDLER

#include <wx/stream.h>
#include <memory>

inline char DUMMY_BUFFER;
class Handler 
{
    public:     
        Handler() {};
        Handler( const wxString& path ) {};
        virtual ~Handler() {};

        // return handler of Parent's Directory
        virtual Handler* GetParent() = 0;

        // return all directory and files in current dir
        virtual wxArrayString& GetChild() = 0;

        // return opened path
        virtual wxString GetName() = 0;
        virtual wxString GetFromCurrent( int step ) = 0;
        // return next or prev folder / file
        virtual wxString GetNext() = 0;
        virtual wxString GetPrev() = 0;

        virtual void Open( const wxString& path) = 0;
        // enumerate current opened file or dir
        virtual void Traverse( bool GetStream = false ) = 0 ;

        virtual bool IsExist( size_t index ) = 0;

        // search from files
        // Index, ItemName, and Item should 
        // return pos, filename, and stream respectively
        virtual size_t Index( const wxString& name ) = 0;

        virtual wxString ItemName( size_t idx ) = 0;
        // return files stream;
        virtual std::shared_ptr<wxInputStream> Item( size_t index ) = 0;

        virtual size_t Size() = 0;
        virtual void Clear() = 0;
};

#endif