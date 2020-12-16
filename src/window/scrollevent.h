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

#ifndef FMR_WINDOW_SCROLLEVENT
#define FMR_WINDOW_SCROLLEVENT

#include <wx/event.h>

class ScrollWinEvent;

wxDECLARE_EVENT( EVT_FMR_SCROLLWIN, ScrollWinEvent );
wxDECLARE_EVENT( EVT_FMR_SCROLLWIN_TOP, ScrollWinEvent );
wxDECLARE_EVENT( EVT_FMR_SCROLLWIN_BOTTOM, ScrollWinEvent );
wxDECLARE_EVENT( EVT_FMR_SCROLLWIN_LINEUP, ScrollWinEvent );
wxDECLARE_EVENT( EVT_FMR_SCROLLWIN_LINEDOWN, ScrollWinEvent );
wxDECLARE_EVENT( EVT_FMR_SCROLLWIN_PAGEUP, ScrollWinEvent );
wxDECLARE_EVENT( EVT_FMR_SCROLLWIN_PAGEDOWN, ScrollWinEvent );
wxDECLARE_EVENT( EVT_FMR_SCROLLWIN_THUMBTRACK, ScrollWinEvent );
wxDECLARE_EVENT( EVT_FMR_SCROLLWIN_THUMBRELEASE, ScrollWinEvent );
wxDECLARE_EVENT( EVT_FMR_SCROLLWIN_DRAG, ScrollWinEvent );
wxDECLARE_EVENT( EVT_FMR_SCROLLWIN_DEFAULT, ScrollWinEvent );

#define EVT_FMR_SCROLLWiN_ALL(func)  \
    EVT_FMR_SCROLLWIN_TOP(func) \
    EVT_FMR_SCROLLWIN_BOTTOM(func)  \
    EVT_FMR_SCROLLWIN_LINEUP(func)  \
    EVT_FMR_SCROLLWIN_LINEDOWN(func)    \
    EVT_FMR_SCROLLWIN_PAGEUP(func)  \
    EVT_FMR_SCROLLWIN_PAGEDOWN(func)    \
    EVT_FMR_SCROLLWIN_THUMBTRACK(func)  \
    EVT_FMR_SCROLLWIN_THUMBTRACK(func)  \
    EVT_FMR_SCROLLWIN_DRAG(func)    \
    EVT_FMR_SCROLLWIN_DEFAULT(func)


class ScrollWinEvent
    : public wxEvent
{
    DECLARE_DYNAMIC_CLASS( ScrollEvent );
    ScrollWinEvent();
    ScrollWinEvent( wxEventType type, int position, wxOrientation orient, wxWindow *win = (wxWindow*) NULL );
    wxEvent *Clone() const { return new ScrollWinEvent(*this); }

    void SetOrientation( wxOrientation orient );
    wxOrientation GetOrientation() const;

    void SetPosition( int pos );
    int GetPosition() const;

    protected:
        int m_position;
        wxOrientation m_orientation;
};

#endif