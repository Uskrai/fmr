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

#include "window/scrollevent.h"
#include <wx/window.h>

IMPLEMENT_DYNAMIC_CLASS( ScrollWinEvent, wxEvent )

wxDEFINE_EVENT( EVT_FMR_SCROLLWIN, ScrollWinEvent );
wxDEFINE_EVENT( EVT_FMR_SCROLLWIN_TOP, ScrollWinEvent );
wxDEFINE_EVENT( EVT_FMR_SCROLLWIN_BOTTOM, ScrollWinEvent );
wxDEFINE_EVENT( EVT_FMR_SCROLLWIN_LINEUP, ScrollWinEvent );
wxDEFINE_EVENT( EVT_FMR_SCROLLWIN_LINEDOWN, ScrollWinEvent );
wxDEFINE_EVENT( EVT_FMR_SCROLLWIN_PAGEUP, ScrollWinEvent );
wxDEFINE_EVENT( EVT_FMR_SCROLLWIN_PAGEDOWN, ScrollWinEvent );
wxDEFINE_EVENT( EVT_FMR_SCROLLWIN_THUMBTRACK, ScrollWinEvent );
wxDEFINE_EVENT( EVT_FMR_SCROLLWIN_THUMBRELEASE, ScrollWinEvent );
wxDEFINE_EVENT( EVT_FMR_SCROLLWIN_DRAG, ScrollWinEvent );
wxDEFINE_EVENT( EVT_FMR_SCROLLWIN_DEFAULT, ScrollWinEvent );

ScrollWinEvent::ScrollWinEvent( wxEventType type, int position, wxOrientation orient, wxWindow *win )
{
    SetEventType( type );
    SetEventObject( win );
    SetOrientation( orient );
    SetPosition( position );
}

void ScrollWinEvent::SetPosition( int pos )
{
    m_position = pos;
}

void ScrollWinEvent::SetOrientation( wxOrientation orient )
{
    m_orientation = orient;
}

int ScrollWinEvent::GetPosition() const
{
    return m_position;
}

wxOrientation ScrollWinEvent::GetOrientation() const
{
    return m_orientation;
}