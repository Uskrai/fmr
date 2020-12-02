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


#include <wx/panel.h>
#include "base/range.h"

#include "bitmap/bitmap.h"

class Config;
class Handler;
class HandlerFactory;

namespace Reader
{

class Thread;

class Window : public wxPanel
{
    // void Error( wxSize size );
    public:
        Window( wxWindow *parent, wxWindowID id=wxID_ANY, 
                const wxPoint &pos=wxDefaultPosition, 
                const wxSize &size=wxDefaultSize, 
                long style=wxTAB_TRAVERSAL, 
                const wxString &name=wxPanelNameStr
                );
        Window( wxWindow* parent, wxSize size );
        ~Window();
        void Open( const wxString& path );
        
        void ReloadConfig();

        void ChangeFolder( int step );
        void Next();
        void Prev();

        void Clear();

        Handler* GetHandler() {return m_fileHandler;}
    private:
        //vector to bitmaps
        Bitmap* m_bitmap;
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
        void OnDraw( wxPaintEvent &event );
        // void OnMouseMotion( wxMouseEvent& event );
        // void OnMouseWheel( wxMouseEvent& event );
        // void OnKeyDown( wxKeyEvent& event );
        
        // return is ViewStart Changed 
        // BITMAP_PAGES OnArrow( wxOrientation orient, int modifier, bool isInstant = false);
        // return is Change Folder
        BITMAP_PAGES OnEdge( int modifier, bool isInstant = false, int onEdgeCount = 0 );
};

};
#endif