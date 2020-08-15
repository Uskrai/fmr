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

class wxDir;

class DefaultHandler
    : public Handler
{
    public:
        DefaultHandler() {} ;
        DefaultHandler( const wxString& string );
        ~DefaultHandler();

        void Open ( const wxString& string );
        void Traverse();

        bool IsExist( int idx );

        int Index( const wxString& path );
        wxInputStream* Item( int idx ) { return stream.at(idx); };

        int Size() { return stream.size(); };

        void Clear();

        static bool CanHandle ( const wxString& path ) { return true; }

   private:
        wxString filename;
        
        void GetAllFiles( wxDir& dir, bool& cont, wxString& filename, wxArrayString& array  );
        void GetAllFiles( const wxString& path, wxVector<wxInputStream*>& stream );

        wxVector<wxInputStream*> stream;

};

#endif