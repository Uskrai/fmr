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

class Handler 
{
    public:     
        Handler() {};
        Handler( const wxString& path ) {};
        virtual ~Handler() {};

        virtual Handler* GetParent() = 0;
        
        virtual wxString GetName() = 0;
        virtual wxString GetNext() = 0;
        virtual wxString GetPrev() = 0;

        virtual void Open( const wxString& path) = 0;
        virtual void Traverse() = 0 ;

        virtual bool IsExist( size_t index ) = 0;

        virtual int Index( const wxString& name ) = 0;
        virtual wxInputStream* Item( size_t index ) = 0;

        virtual size_t Size() = 0;
        virtual void Clear() = 0;
};

#endif