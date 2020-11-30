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
#include "config.h"

#include <wx/scrolwin.h>
#include <wx/image.h>
#include <wx/bitmap.h>
#include <wx/vector.h>
#include <wx/thread.h>
#include <wx/sharedptr.h>


namespace Reader
{

enum VectorPos
{
    VECTOR_PUSH = -1,   // used when using vector.push_back(val)
    VECTOR_BEGIN        // used when using vector.insert(vector.begin() val) 
};

class Image :
    public wxThreadHelper
{
        typedef wxSharedPtr<wxBitmap> BitmapPtr;
        typedef wxSharedPtr<wxImage> ImagePtr;
        typedef wxVector<BitmapPtr> VectorBitmap;
        typedef wxVector<ImagePtr> VectorImage;

    public:
        Image( wxScrolledWindow* parent );
        ~Image();
        void Open( wxString path );
        VectorBitmap Get(); // return all Loaded Bitmap
        void Clear(); // clear all value
        wxScrolledWindow* GetParent() { return this->parent; }

        // return imagePos X and Y as wxPoint
        wxPoint GetPosition( int index) { return wxPoint(this->imagePosX.at(index), this->imagePosY.at(index) );}

    private:
        wxScrolledWindow* parent;

        Config* config = Config::Get();

        void Load( wxString path ); // load image and
        bool LoadAt( int index, VectorPos vectorPos = VECTOR_PUSH ); // load image at given index and whether to push or insert
        void LoadBitmap( ImagePtr img, VectorPos pos = VECTOR_PUSH ); // load bitmap from given image and whether to push or insert

        bool isLoadBitmap = false; // turned on when loading Bitmap immediately in multi-thread
        bool isPreLoadImage = false; // turned on when preLoading Image in multi-thread

        void RefreshImagePosition(); // recalculate all image position
        void AddPosition( const BitmapPtr& bmp, wxVector<int>& x, wxVector<int>& y); // calculate position from given bitmap
        void AddPosition( const BitmapPtr& bmp ); // overrided function of above used to add position to imagePos X and Y
        int GetCenteredPosition( int width ); // Get Centered Position of given Bitmap


        wxThread::ExitCode Entry();
        wxCriticalSection gCS; // useed for locking used variable to make thread-safe
        bool TestDestroy() { return this->GetThread()->TestDestroy(); }

        Traverser* files = new Traverser();
        wxString file;

        VectorImage image;
        int prev, next, showPrev, showNext, cachePrev,cacheNext, maxWidth = 0;
        VectorBitmap bitmap;
        wxVector<int> imagePosY, imagePosX;
        // wxVector<wxPoint> imagePos;
};

};

#endif 