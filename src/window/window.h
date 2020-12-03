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

#ifndef FMR_WINDOW_WINDOW
#define FMR_WINDOW_WINDOW

#include <wx/window.h>
class wxScrollBar;

class ScrolledWindow : public wxWindow
{
    public:
        ScrolledWindow() : wxWindow(){};
        ScrolledWindow( wxWindow *parent, 
                        wxWindowID id = wxID_ANY, 
                        const wxPoint &pos = wxDefaultPosition,
                        const wxSize &size = wxDefaultSize,
                        long style = 0,
                        const wxString &name = wxPanelNameStr 
                    );
        ~ScrolledWindow();
        bool Create( wxWindow *parent,
                wxWindowID id = wxID_ANY,
                const wxPoint &pos = wxDefaultPosition,
                const wxSize &size = wxDefaultSize,
                long style = 0,
                const wxString &name = wxPanelNameStr
            );
        
        // to scroll window
        void Scroll( const wxPoint &pos );
        void Scroll( int x, int y );
        // to scroll window,
        // step will be added by 
        // ViewStart according to orientation.
        void Scroll( wxOrientation orient, int step );

        wxPoint GetViewStart() { return m_viewStart; }
    
        void AdjustScrollBar();
    protected:
        wxScrollBar *m_vScrollBar = NULL, *m_hScrollBar = NULL;
        wxPoint m_viewStart;
        // this is how much window should scroll per event.
        int m_stepPerKey = 300, m_stepPerWheel = 300;
        void DoSetVirtualSize( int width, int height );
        
        void DoPrepareDC( wxDC &dc );
    
    private:
        void OnSize( wxSizeEvent &event );
        void OnPaint( wxPaintEvent &event );
        void OnKeyDown( wxKeyEvent &event );
        void OnMouseWheel( wxMouseEvent &event );
        void OnMouseMotion( wxMouseEvent &event );
        virtual void OnDraw( wxDC &dc ) {};

        wxDECLARE_EVENT_TABLE();
        
};

#endif