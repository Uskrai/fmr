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
class Bitmap
{
    public:
        Bitmap( wxScrolledWindow* parent );
        ~Bitmap();
        void Add( const wxImage& image, int pos );

        void SetName( size_t idx, wxString name )
            { GetAll()[idx].SetName(name); }

        void SetFlags( int posFlags, int sizeFlags )
            { SetPosition(posFlags);SetSize(sizeFlags);}
        void SetSize( int flags ) 
            { m_flagSize = flags; }
        void SetPosition( int flags ) 
            { m_flagPosition = flags; }
            
        void SetLimit( size_t limit );
        void Clear();

        void Refresh();
        void RefreshPosition( );
        void RefreshSize( );

        bool ChangePage( int step );
        bool IsImageOk( int pos );
        bool NextPage();
        bool PrevPage();

        wxVector<SBitmap>& GetAll() { return m_item; }
        
        wxVector<SBitmap*>& Get() { return m_itemPage; }

        SBitmap* Get( const wxPoint& area, const wxPoint& position );

        wxVector<SBitmap*> Get( const wxPoint& area, const wxSize& size );

    protected:
        wxScrolledWindow* m_parent = NULL;
        wxVector<SBitmap> m_item;
        wxVector<SBitmap*> m_itemPage;

    private:
        size_t m_limit = 1;
        size_t m_posFirst = 0 , m_posLast = 0;

        int m_flagSize, m_flagPosition;
        int m_maxWidth, m_maxHeight;

        wxScrolledWindow* GetParent() { return m_parent; }
        void Prepare( const wxImage& image, int pos, struct SBitmap& bmp );
        void Exit( int i );

        int Push( struct SBitmap& bmp );

        int Centered( int width );
};