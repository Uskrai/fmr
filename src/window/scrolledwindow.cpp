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

#include <fmr/window/scrolledwindow.h>
#include <fmr/common/event.h>
#include <fmr/common/dimension.h>
#include <wx/scrolbar.h>
#include <wx/dc.h>
#include <wx/dcclient.h>
#include <wx/event.h>

ScrolledWindow::ScrolledWindow( wxWindow *parent,
                                wxWindowID id,
                                const wxPoint &pos,
                                const wxSize &size,
                                long style,
                                const wxString &name
                            )
    : wxScrolledCanvas()
{
    Create( parent, id, pos, size, style, name );
}

ScrolledWindow::~ScrolledWindow()
{
}

bool ScrolledWindow::Create(    wxWindow *parent,
                                wxWindowID id,
                                const wxPoint &pos,
                                const wxSize &size,
                                long style,
                                const wxString &name
                            )
{
    bool ret = wxScrolledCanvas::Create( parent, id,  pos, size, style, name );
    m_timer.SetOwner( this, ScrolledTimerID );
    CreateScrollBar( wxBOTH );
    SetVirtualSize(0,0);
    BindEvent();
    return ret;
}

void ScrolledWindow::BindEvent()
{
    std::vector<wxEventTypeTag<wxScrollWinEvent>> scroll_win_event = {
        wxEVT_SCROLLWIN_TOP,
        wxEVT_SCROLLWIN_BOTTOM,
        wxEVT_SCROLLWIN_LINEUP,
        wxEVT_SCROLLWIN_LINEDOWN,
        wxEVT_SCROLLWIN_PAGEUP,
        wxEVT_SCROLLWIN_PAGEDOWN,
        wxEVT_SCROLLWIN_THUMBTRACK,
        wxEVT_SCROLLWIN_THUMBRELEASE
    };

    // bind the scroll_event first, i don't know why
    // the event order is descending
    event::Bind( scroll_win_event, &ScrolledWindow::OnScroll, this );

    Bind( wxEVT_SIZE, &ScrolledWindow::OnSize, this );
    Bind( wxEVT_PAINT, &ScrolledWindow::OnPaint, this );
    Bind( wxEVT_KEY_DOWN, &ScrolledWindow::OnKey, this );
    Bind( wxEVT_MOUSEWHEEL, &ScrolledWindow::OnMouseWheel, this );
    Bind( wxEVT_MOTION, &ScrolledWindow::OnMouseMotion, this );
    Bind( wxEVT_SCROLLWIN_LINEUP, &ScrolledWindow::OnScrollLine, this );
    Bind( wxEVT_SCROLLWIN_LINEDOWN, &ScrolledWindow::OnScrollLine, this );
    Bind( wxEVT_SCROLLWIN_THUMBTRACK, &ScrolledWindow::OnScrollThumbTrack, this );
    Bind( wxEVT_TIMER, &ScrolledWindow::OnSetVirtualSize, this, ScrolledTimerID );
}

void ScrolledWindow::CreateScrollBar( wxOrientation orient )
{
    SetScrollbars(
        1,1,1,1
    );
}

void ScrolledWindow::OnSize( wxSizeEvent &event )
{
    AdjustScrollBar();
    event.Skip();
}

// void ScrolledWindow::DoPrepareDC( wxDC &dc )
// {
    // dc.SetDeviceOrigin( -GetViewStart().x, -GetViewStart().y );
// }

void ScrolledWindow::OnPaint( wxPaintEvent &event )
{
    wxPaintDC dc(this);
    DoPrepareDC( dc );
    OnDraw( dc );
}

void ScrolledWindow::OnScroll( wxScrollWinEvent &event )
{
    wxOrientation orient = dimension::GetOrient( event.GetOrientation() );
    if ( orient == wxVERTICAL )
        DoScroll( -1, event.GetPosition() );
    else if ( orient == wxHORIZONTAL )
        DoScroll( event.GetPosition(), -1 );
}

void ScrolledWindow::AdjustScrollBar()
{
    AdjustScrollbars();
}

int ScrolledWindow::GetScrollPos( const wxOrientation &orient ) const
{
    int pos = dimension::Get( GetViewStart(), orient );
    int pos_limit = GetScrollRangeLimit( orient );

    int ret = pos < pos_limit ? pos : pos_limit;
    ret = ret > 0 ? ret : 0;
    return ret;
}

int ScrolledWindow::GetScrollRangeLimit( const wxOrientation &orient ) const
{
    return GetScrollRange( orient ) - GetScrollThumb( orient );
}

void ScrolledWindow::OnSetVirtualSize( wxTimerEvent &event )
{
    AdjustScrollBar();
}

void ScrolledWindow::DoSetVirtualSize( int width, int height )
{
    m_virtualSize = wxSize(width,height);
    if ( ! m_timer.IsRunning() )
        m_timer.StartOnce( 500 );
}

void ScrolledWindow::Scroll( const wxPoint &pos )
{
    DoScroll( pos.x, pos.y );
}

void ScrolledWindow::Scroll( int x, int y )
{
    Scroll( wxPoint( x, y ) );
}

void ScrolledWindow::Scroll( wxOrientation orient, int step )
{
    int ver_pos = GetScrollPos( wxVERTICAL );
    int hor_pos = GetScrollPos( wxHORIZONTAL );
    wxPoint pos( hor_pos, ver_pos );

    int orient_pos = dimension::Get( pos, orient );
    int dest_pos = orient_pos + step;
    int limit_pos = GetScrollRangeLimit( orient );
    dest_pos = dest_pos > limit_pos ? limit_pos : dest_pos;
    dest_pos = dest_pos < 0 ? 0 : dest_pos;

    dimension::Set( pos, orient, dest_pos );
    Scroll( pos );
}

void ScrolledWindow::DoScrollLine( wxEventType type, wxOrientation orient, int step )
{
    wxQueueEvent(
        this,
        new wxScrollWinEvent(
            type,
            GetScrollPos( orient ) + step,
            orient
        )
    );
}

void ScrolledWindow::LineUp( wxOrientation orient, int step )
{
    DoScrollLine(
        wxEVT_SCROLLWIN_LINEUP,
        orient,
        -step
    );
}

void ScrolledWindow::LineDown( wxOrientation orient, int step )
{
    DoScrollLine(
        wxEVT_SCROLLWIN_LINEDOWN,
        orient,
        step
    );
}

void ScrolledWindow::OnScrollLine( wxScrollWinEvent &event )
{
    wxOrientation orient = dimension::GetOrient( event.GetOrientation() );
    int bottom_edge = GetScrollRangeLimit( orient );
    int scroll_pos = GetScrollPos( orient );
    int pos = event.GetPosition();
    int step = pos - scroll_pos;
    wxDirection direction = dimension::GetDirection( orient, step );
    bool is_top = scroll_pos == 0;

    bool is_bottom = scroll_pos == bottom_edge;
    is_bottom = is_bottom || GetScrollRange( orient ) < dimension::Get( GetClientSize(), orient );

    bool is_below_bottom = is_bottom && pos >= bottom_edge;
    bool is_over_top = is_top && pos <= 0;
    bool is_edge = (is_top && is_over_top) || (is_bottom && is_below_bottom );

    if ( !is_edge || direction == wxALL )
        event.Skip();
    else
    {
        // this will inverse the direction
        // if m_isFromRight is true
        // and direction is left or right
        if ( m_isFromRight && (direction == wxLEFT || direction == wxRIGHT) )
            direction = ( direction == wxLEFT ) ?
                wxRIGHT : wxLEFT;

        return OnEdge( direction );
    }
}

void ScrolledWindow::OnKey( wxKeyEvent &event )
{
    bool isUp = event.GetKeyCode() == WXK_UP;
    bool isDown = event.GetKeyCode() == WXK_DOWN;
    bool isLeft = event.GetKeyCode() == WXK_LEFT;
    bool isRight = event.GetKeyCode() == WXK_RIGHT;

    // only scroll for default if there is
    // no modifier ( ctrl,alt,shift, etc )
    // is pressed
    if ( ! event.HasAnyModifiers() )
    {
        if ( isUp || isDown || isLeft || isRight )
        {
            wxOrientation orient;

            // set orientation to vertical
            // if key is up or down
            if ( isUp || isDown )
                orient = wxVERTICAL;

            // set orientation to horizontal
            // if key is left or right
            if ( isLeft || isRight )
                orient = wxHORIZONTAL;

            // if key is up or left
            // then lineup
            if ( isUp || isLeft )
                LineUp( orient, m_stepPerKey );

            // if key is down or right
            // then linedown
            if ( isDown || isRight )
                LineDown( orient, m_stepPerKey );

            return;
        }
    }

    event.Skip();
}

void ScrolledWindow::OnMouseWheel( wxMouseEvent &event )
{
    wxOrientation orient = wxVERTICAL;
    // make horizontal if shift is down
    orient = ( event.ShiftDown() )
        ? wxHORIZONTAL : orient;

    int step = ( event.GetWheelDelta() / event.GetWheelRotation() ) * -1;
    // multiply by negative if invert
    // to reverse it
    step *= ( m_isMouseWheelInvert ) ? -1 : 1;

    if ( step < 0 )
        LineUp( orient, m_stepPerWheel );
    else if ( step > 0 )
        LineDown( orient, m_stepPerWheel );
}

void ScrolledWindow::OnMouseMotion( wxMouseEvent &event )
{
    // TODO: make this member var
    static wxPoint lastPos;
    // only scroll for default if there is
    // no modifier ( ctrl,alt,shift,etc )
    // is pressed
    if ( ! event.HasAnyModifiers() )
    {
        if ( event.Dragging() && event.LeftIsDown() && ! event.RightIsDown() && ! event.MiddleIsDown() )
        {
            wxPoint view = GetViewStart();
            int view_x = event.GetX() - lastPos.x;
            int view_y = event.GetY() - lastPos.y;

            view.x -= view_x;
            view.y -= view_y;

            Scroll( view );
            return;
        }
    }

    // store last mouse position
    // to track movement
    lastPos = event.GetPosition();
    event.Skip();
}

void ScrolledWindow::OnScrollThumbTrack( wxScrollWinEvent &event )
{
    event.Skip();
}

