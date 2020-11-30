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

#ifndef MyReader
#define MyReader

#include "reader/image.h"

#include <wx/scrolwin.h>
#include <wx/stattext.h>
#include <wx/sizer.h>
#include <wx/dc.h>

namespace Reader
{

class Reader : public wxScrolledWindow
{
    // void Error( wxSize size );
    public:
        Reader( wxWindow* parent, wxSize size );
        Reader( wxWindow* parent, wxSize size, wxString path);
        void LoadImage( wxString path );

    private:
        Image* image = new Image(this);
        int w,h;
        void Error( wxSize size ); 
        void OnDraw( wxDC& dc );

};

};
#endif