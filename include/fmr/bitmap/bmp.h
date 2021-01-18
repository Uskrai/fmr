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

#ifndef FMR_BITMAP_BMP
#define FMR_BITMAP_BMP

#include <wx/bitmap.h>

namespace fmr
{

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
    BITMAP_ALLOWENLARGE = 0x20,

    BITMAP_SIZE_INVALID = BITMAP_ORIGINAL | BITMAP_FITWIDTH | BITMAP_FITHEIGHT | BITMAP_FITALL | BITMAP_MANUAL | BITMAP_ALLOWENLARGE
};

enum BITMAP_PAGES : int
{
    BITMAP_CHANGEPAGE,
    BITMAP_ENDOFPAGE,
    BITMAP_NOTLOADED,
    BITMAP_NOTCHANGED
};

struct SBitmap
{
    wxBitmap m_item = wxBitmap( wxSize(1,1) );
    wxString m_name = wxEmptyString;
    size_t m_index = -1;
    float m_scale;
    wxPoint m_pos = wxPoint(0,0);
    bool m_isOk = false; // determine bitmap status
    bool m_isLoaded = false;

    SBitmap(){}
    SBitmap( bool isLoaded );
    const wxBitmap& GetBitmap() const;
    wxBitmap& GetBitmap();
    bool IsOk() const;
    bool IsLoaded() const;
    
    bool IsPointed( const wxPoint& area, const wxPoint& position ) const;
    bool IsShown( const wxPoint& area, const wxSize& size ) const;

    wxPoint GetPosition()  const;
    wxSize GetSize() const;

    wxString GetName();
    size_t GetIndex();
    float GetScale();
    int GetWidth() const;
    int GetHeight() const;
    int GetY() const;
    int GetX() const;

    void SetBitmap( const wxBitmap& bmp );
    void SetLoaded( bool stat = true );
    void SetName( const wxString& name );
    void SetIndex( size_t idx );
    void SetScale( float scale );
    void SetPosition( const wxPoint& pos );
    void SetY( int PosY );
    void SetX( int PosX );
};

}; // namespace fmr

#endif
