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

#ifndef READER_IMAGE
#define READER_IMAGE

#include "traverser.h"

#include <wx/window.h>
#include <wx/image.h>
#include <wx/bitmap.h>
#include <wx/vector.h>
#include <wx/thread.h>
#include <wx/sharedptr.h>


namespace Reader
{

enum VectorPos
{
    VECTOR_PUSH = -1,
    VECTOR_BEGIN
};

class Image :
    public wxThreadHelper
{
        typedef wxSharedPtr<wxBitmap> BitmapPtr;
        typedef wxSharedPtr<wxImage> ImagePtr;
        typedef wxVector<BitmapPtr> vectorBitmap;
        typedef wxVector<ImagePtr> vectorImage;

    public:
        Image( wxWindow* parent );
        void Open( wxString path );

    private:
        wxWindow* parent;

        bool isLoadBitmap = false;
        void Load( wxString path );
        bool LoadAt( int index, VectorPos vectorPos = VECTOR_PUSH );
        void LoadBitmap( ImagePtr img, VectorPos pos ) { }

        bool isPreLoadImage = false;
        void PreLoadImage();

        wxThread::ExitCode Entry();
        bool TestDestroy() { return this->GetThread()->TestDestroy(); }

        Traverser* files = new Traverser();
        wxString file;

        vectorImage image;
        int prev, next;
        vectorBitmap bitmap;
};

};

#endif