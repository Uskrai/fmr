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

#include <wx/bitmap.h>

enum BITMAP_POSITION : int
{
    BITMAP_VERTICAL     = 0x1,
    BITMAP_WEBTOON      = 0x2,
    BITMAP_HORIZONTAL   = 0x4,
    BITMAP_CENTERED     = 0x8,
    
    INVALID_BITMAP_POSITION = 0x10
};

enum BITMAP_SIZE : int
{
    BITMAP_ORIGINAL     = 0x1,
    BITMAP_FITWIDTH     = 0x2,
    BITMAP_FITHEIGHT    = 0x4,
    BITMAP_FITALL       = 0x8,
    BITMAP_MANUAL       = 0x10,

    INVALID_BITMAP_SIZE    = 0x20
};

struct SBitmap
{
    wxBitmap m_item = wxBitmap( wxSize(1,1) );
    wxString m_name = wxEmptyString;
    wxPoint m_pos = wxPoint(0,0);
    bool m_isOk = false; // determine bitmap status
    bool m_isLoaded = false;

    SBitmap(){}
    SBitmap( bool isLoaded ) { m_isLoaded = isLoaded; }
    const wxBitmap& GetBitmap() const { return this->m_item; }
    wxBitmap& GetBitmap() { return m_item; }
    bool IsOk() const { return m_isOk; }
    bool IsLoaded() const { return m_isLoaded; }
    
    bool IsPointed( const wxPoint& area, const wxPoint& position ) const
    {
        int posY = area.y + position.y,
            posX = area.x + position.x,
            bmpPosY = GetY(), bmpAfterY = bmpPosY + GetHeight(),
            bmpPosX = GetX(), bmpAfterX = bmpPosX + GetWidth();

            return  posY >= bmpPosY && posY <= bmpAfterY 
                && posX >= bmpPosX && posX <= bmpAfterX;
    }

    bool IsShown( const wxPoint& area, const wxSize& size ) const
    {
        int
        top = area.y, bottom = top + size.GetHeight(),
        left = area.x, right = left + size.GetHeight(),
        bmpPosY = GetY(), bmpAfterY = bmpPosY + GetHeight(),
        bmpPosX = GetX(), bmpAfterX = bmpPosX + GetWidth();

        return    
            (   ( bmpPosY >= top || bmpAfterY >= top )
            &&  ( bmpPosY <= bottom || bmpAfterY <= bottom )  )
            &&  ( ( bmpPosX >= left || bmpAfterY >= left )
            ||  ( bmpPosX <= right || bmpAfterX <= right ) ) ;
    }

    wxPoint GetPosition()  const { return m_pos; }
    wxSize GetSize() const { return wxSize( GetWidth(), GetHeight() ); }

    wxString GetName() { return m_name;}
    int GetWidth() const { return m_item.GetWidth(); }
    int GetHeight() const { return m_item.GetHeight(); }
    int GetY() const { return m_pos.y; }
    int GetX() const { return m_pos.x; }

    void SetBitmap( const wxBitmap& bmp ) { m_item = bmp; m_isOk = true; SetLoaded(); }
    void SetLoaded( bool stat = true ) { m_isLoaded = stat; }
    void SetName( const wxString& name ) { m_name = name;}
    void SetPosition( const wxPoint& pos ) { m_pos = pos; }
    void SetY( int PosY ) { m_pos.y = PosY; }
    void SetX( int PosX ) { m_pos.x = PosX; }
};

#endif