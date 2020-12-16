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


#include "window/scrolledwindow.h"


#include "base/range.h"
#include "bitmap/bitmap.h"
#include "thread/thread.h"
#include "reader/load_reader.h"
#include "reader/zoom_reader.h"
#include "handler/abstract_handler.h"


#include <wx/event.h>
#include <memory>

namespace fmr
{

class Config;

namespace reader
{

enum ScrollingType : int
{
    SCROLL_BY_WINDOW,
    SCROLL_BY_IMAGE,
    SCROLL_BY_PIXEL
};

class LoadThread;

class Window : public ScrolledWindow
{
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

        std::shared_ptr<AbstractHandler> GetHandler() {return m_fileHandler;}
        void DoSetNull( int id );
    protected:
        void AdjustBitmap();
    private:
        // reference to threading class
        LoadThread *m_loadThread = NULL;
        ZoomThread *m_zoomThread = NULL;
        // pointer to handler
        std::shared_ptr<AbstractHandler> m_fileHandler = NULL;
        //vector to bitmaps
        std::shared_ptr<Bitmap> m_bitmap = NULL;
        // pointer to config files 
        Config* m_config;
        // pointer to scrollbar
        wxSizer *m_sizer;
        bool m_isOpened = false;

        template<typename T>
        T ConfRead( wxString name, T def );

        void CalcScrollStep( ScrollingType type );

        std::shared_ptr<AbstractHandler> NewHandler( const wxString &path );
        std::shared_ptr<Bitmap> NewBitmap( size_t size, size_t limit );

        void Error( wxSize size ); 
        void OnDraw( wxDC& dc );
        wxDECLARE_EVENT_TABLE();

        void OnSize( wxSizeEvent &event );
        void OnMouseMotion( wxMouseEvent &event );
        void OnEdge( wxDirection direction );

        void OnThreadUpdate( wxCommandEvent &event );
        void OnZoomThreadCompleted( wxCommandEvent &event ) { AdjustScrollBar(); };
        
};

}; // namespace reader

}; // namespace fmr
#endif