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

#ifndef BITMAP
#define BITMAP

#include "base/vector.h"
#include <wx/bitmap.h>

class wxBitmap;
class wxImage;

typedef Vector<wxBitmap> BmpVector;
class BaseBitmap 
    : public BmpVector
{
    public:
        void Add ( const wxImage& image, VectorPos position )
        {
            wxBitmap bmp;
            this->OnPrepare( image, bmp );

            BmpVector::Add( bmp, position );

            switch ( position )
            {
                case VECTOR_BEGIN:
                    this->AddPosition(bmp);
                    break;
                default:
                    this->RefreshPosition(bmp);
            }

            this->OnExit(bmp);
        }
        
        virtual ~BaseBitmap() {}

    private:
        virtual void OnPrepare( const wxImage& image, wxBitmap& bmp) = 0 ;
        virtual void AddPosition( const wxBitmap& bmp ) = 0 ;
        virtual void RefreshPosition( const wxBitmap& bmp ) = 0 ;
        virtual void OnExit( wxBitmap& bmp ) = 0 ;

        Vector<int> posY, posX;
};

#endif