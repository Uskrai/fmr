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


#include "window/window.h"
#include <wx/event.h>

#include "base/range.h"

#include "bitmap/bitmap.h"

class Config;
class Handler;
class HandlerFactory;
class wxScrollBar;
class wxSizer;

namespace Reader
{

class LoadThread;

class Window : public ScrolledWindow
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
        bool Destroy();

        bool Open( const wxString& path );
        
        void ReloadConfig();

        bool ChangeFolder( int step );
        void Next();
        void Prev();

        void Clear();

        Handler* GetHandler() {return m_fileHandler;}
    private:
        //vector to bitmaps
        Bitmap* m_bitmap = NULL;
        // reference to threading class
        LoadThread *m_thread = NULL;
        // pointer to handler
        Handler* m_fileHandler = NULL;
        HandlerFactory* m_factory = NULL;
        // pointer to config files 
        Config* m_config;
        // pointer to scrollbar
        wxScrollBar *m_vScroll, *m_hScroll;
        wxSizer *m_sizer;

        void Find( const wxString& path );

        template<typename T>
        T ConfRead( wxString name, T def );

        Handler *NewHandler( const wxString &path );
        Bitmap *NewBitmap( size_t size, size_t limit );

        void Error( wxSize size ); 
        void OnDraw( wxDC& dc );
        wxDECLARE_EVENT_TABLE();

        void OnEdge( wxDirection direction );

        void OnThreadUpdate( wxCommandEvent &event ) { Refresh();};
        void OnThreadComplete( wxCommandEvent &event );
        
};

};
#endif