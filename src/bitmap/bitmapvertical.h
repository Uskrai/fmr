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
#include "bitmap/bmp.h"

class wxScrolledWindow;
class BitmapVertical
{
    public:
        BitmapVertical( wxScrolledWindow* parent );
        ~BitmapVertical();
        void Add( const wxImage& image, VectorPos pos );

        // getting bitmap
        wxVector<SBitmap>& Get();

        void Clear();
    protected:
        int m_maxWidth, m_maxHeight;
        wxVector<SBitmap> m_item;
        wxScrolledWindow* m_parent;
        wxScrolledWindow* GetParent() { return m_parent; }
        void Prepare( const wxImage& image, VectorPos pos, struct SBitmap& bmp );
        void Exit( int i );

        int Push( struct SBitmap& bmp, wxPoint& pos );
        int Insert( struct SBitmap& bmp, wxPoint& pos, VectorPos idx );

        void CalcPosition( int idx, wxPoint& pos );
        int Centered( int width );
        void RefreshPosition( );
};