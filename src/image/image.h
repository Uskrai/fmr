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

#include "base/vector.h"
#include "handler/handler.h"
#include "base/config.h"
#include <wx/image.h>

class wxImage;

class wxScrolledWindow;

#include <wx/thread.h>
class wxImage;

class Image
    : public wxThreadHelper
{
        typedef wxVector<wxBitmap> VectorBitmap;
        typedef wxVector<wxImage> VectorImage;

    public:
        Image( wxScrolledWindow* parent );
        ~Image();
        void Open( const wxString& path );
        // VectorBitmap Get(); // return all Loaded Bitmap
        const wxBitmap& Get( int index ) const { return this->bitmap.at(index); }
        wxBitmap& Get( int index ) { return this->bitmap.at(index); }

        wxVector<wxImage>& Get() { return m_item; }

        int Get ( const wxPoint& area, const wxPoint& posiion ) const;
        wxVector<int> Get( const wxPoint& position, const wxSize& size ) const;
        void Clear(); // clear all value
        wxScrolledWindow* GetParent() const { return this->parent; }

        bool Load( wxInputStream* stream, int pos );
        bool Push( wxInputStream* stream );
        // return imagePos X and Y as wxPoint
        wxPoint GetPosition( int index ) const { return wxPoint(this->imagePosX.at(index), this->imagePosY.at(index) );}

        // wxCriticalSection& GetLock() { return this->gCS; }
    private:
        wxScrolledWindow* parent;
        wxVector<wxImage> m_item;

        Config* config = Config::Get();

        void Load( const wxString& path ); // load image and
        bool LoadAt( int index, VectorPos vectorPos = VECTOR_END ); // load image at given index and whether to push or insert
        void LoadBitmap( const wxImage& img, VectorPos pos = VECTOR_END ); // load bitmap from given image and whether to push or insert

        bool isThreadLoadBitmap = false; // turned on when loading Bitmap immediately in multi-thread
        bool isThreadPreLoadImage = false; // turned on when preLoading Image in multi-thread
        void ThreadLoadBitmap();
        void ThreadPreLoadImage();

        void RefreshImagePosition(); // recalculate all image position
        void AddPosition( const wxBitmap& bmp, wxVector<int>& x, wxVector<int>& y); // calculate position from given bitmap
        void AddPosition( const wxBitmap& bmp ); // overrided function of above used to add position to imagePos X and Y
        int GetCenteredPosition( int width ); // Get Centered Position of given Bitmap


        wxThread::ExitCode Entry();
        // wxCriticalSection gCS; // useed for locking used variable to make thread-safe
        bool TestDestroy() { return true; }

        Handler* files;
        wxString file;

        VectorImage image;
        int prev, next, showPrev, showNext, cachePrev,cacheNext, maxWidth;
        VectorBitmap bitmap;
        wxVector<int> imagePosY, imagePosX;
        // wxVector<wxPoint> imagePos;
};

#endif
