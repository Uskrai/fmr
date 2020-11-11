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

#ifndef FMR_BITMAP_BITMAP
#define FMR_BITMAP_BITMAP

#include <wx/bitmap.h>
#include "bitmap/bmp.h"
#include "base/vector.h"

class Bitmap
{
    public:
        void Clear();
        bool IsExist( int first, int last ) const;

        void SetLimit( size_t limit );

        void Refresh();

        virtual void RefreshPosition() = 0;
        virtual void RefreshSize() = 0;

        void SetName( size_t idx, wxString name )
            { GetAll()[idx].SetName(name); }

        bool ChangePage( int step );
        bool IsImageOk( int pos );
        bool NextPage();
        bool PrevPage();

        wxVector<SBitmap>& GetAll() { return m_item; }
        const wxVector<SBitmap>& GetAll() const { return m_item; }
        
        wxVector<SBitmap*>& Get() { return m_itemPage; }
        const wxVector<SBitmap*>& Get() const { return m_itemPage; }

        const SBitmap* Get( const wxPoint& area, const wxPoint& position ) const;
        SBitmap* Get( const wxPoint& area, const wxPoint& position );

        wxVector<const SBitmap*> Get( const wxPoint& area, const wxSize& size ) const;
        wxVector<SBitmap*> Get( const wxPoint& area, const wxSize& size );

        virtual ~Bitmap() {};

    protected:

        wxVector<SBitmap> m_item;
        wxVector<SBitmap*> m_itemPage;

    private:
        size_t m_limit = 1;
        size_t m_posFirst = 0 , m_posLast = 0;

};

#endif