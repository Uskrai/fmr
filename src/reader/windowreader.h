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

#ifndef FMR_READER_WINDOW
#define FMR_READER_WINDOW


#include <wx/scrolwin.h>
#include "base/range.h"

class Config;
class BitmapVertical;
class Handler;
class HandlerFactory;

namespace Reader
{

class Thread;

class Window : public wxScrolledWindow
{
    // void Error( wxSize size );
    public:
        Window( wxWindow* parent, wxSize size );
        ~Window();
        void Open( wxString path );

        void ChangeFolder( const wxString& path );
        void Next();
        void Prev();

        void Clear();

        Handler* GetHandler() {return m_fileHandler;}
    private:
        //vector to bitmaps
        BitmapVertical* m_bitmap;
        // reference to threading class
        Thread* m_thread;
        // pointer to handler
        Handler* m_fileHandler = NULL;
        HandlerFactory* m_factory = NULL;
        // pointer to config files 
        Config* m_config;

        void Find( const wxString& path );

        template<typename T>
        T ConfRead( wxString name, T def );

        int w,h;
        void Error( wxSize size ); 
        void OnDraw( wxDC& dc );
        wxDECLARE_EVENT_TABLE();
        void OnMouseMotion( wxMouseEvent& event );
        wxPoint m_mousePosition;
        void OnMouseWheel( wxMouseEvent& event );
        void OnKeyDown( wxKeyEvent& event );
        void OnArrow( wxOrientation orient, int modifier);
        
        size_t m_onEdge;
        void OnEdge( int modifier );
};

};
#endif