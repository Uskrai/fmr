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

int main();

#include "bitmap/bmpvertical.h"
#include <wx/scrolwin.h>
#include <wx/bitmap.h>

int main () 
{
    wxInitAllImageHandlers();
    wxImage img("001.jpg");
    wxScrolledWindow* win = new wxScrolledWindow();
    Reader::Bitmap bmp(win);
    bmp.Add( img, VECTOR_END );
    return 0;
}