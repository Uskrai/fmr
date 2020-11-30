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

#include "base/vector.h"
#include "bitmap/bitmap.h"

class wxScrolledWindow;
class BitmapVertical
    : public Bitmap
{
    public:
        BitmapVertical( wxScrolledWindow* parent );
        ~BitmapVertical();
        void Add( const wxImage& image, int pos, bool isScroll = false );

        // void Refresh();
        void RefreshPosition( );
        void RefreshSize( );
    protected:
        int m_maxWidth, m_maxHeight;
        wxScrolledWindow* m_parent;

        wxScrolledWindow* GetParent() { return m_parent; }
        void Prepare( const wxImage& image, int pos, struct SBitmap& bmp );
        void Exit( int i, bool isScroll = false);

        int Push( struct SBitmap& bmp );

        int Centered( int width );
};