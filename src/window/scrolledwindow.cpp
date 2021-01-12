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

#include "window/scrolledwindow.h"
#include <wx/scrolbar.h>
#include <wx/dc.h>
#include <wx/dcclient.h>
#include <wx/event.h>
#include "base/dimension.h"

wxBEGIN_EVENT_TABLE( ScrolledWindow, wxScrolledWindow )
    EVT_SIZE( ScrolledWindow::OnSize )
    EVT_PAINT( ScrolledWindow::OnPaint )
    EVT_KEY_DOWN( ScrolledWindow::OnKey )
    EVT_MOUSEWHEEL( ScrolledWindow::OnMouseWheel )
    EVT_MOTION( ScrolledWindow::OnMouseMotion )
    EVT_SCROLLWIN_LINEUP( ScrolledWindow::OnScrollLine )
    EVT_SCROLLWIN_LINEDOWN ( ScrolledWindow::OnScrollLine )
    EVT_SCROLLWIN_THUMBTRACK( ScrolledWindow::OnScrollThumbTrack )
    EVT_SCROLLWIN( ScrolledWindow::OnScroll )
    EVT_TIMER( ScrolledTimerID, ScrolledWindow::OnSetVirtualSize)
wxEND_EVENT_TABLE()

ScrolledWindow::ScrolledWindow( wxWindow *parent,
                                wxWindowID id,
                                const wxPoint &pos,
                                const wxSize &size,
                                long style,
                                const wxString &name
                            )
    : wxScrolledWindow()
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
    bool ret = wxScrolledWindow::Create( parent, id,  pos, size, style, name );
    m_timer.SetOwner( this, ScrolledTimerID );
    CreateScrollBar( wxBOTH );
    SetVirtualSize(0,0);
    return ret;
}

void ScrolledWindow::CreateScrollBar( wxOrientation orient )
{
    SetScrollbar(
        wxVERTICAL,
        GetViewStart().y,
        GetSize().GetHeight(),
        GetVirtualSize().GetHeight()
    );

    SetScrollbar(
        wxVERTICAL,
        GetViewStart().x,
        GetSize().GetHeight(),
        GetVirtualSize().GetHeight()
    );
}

void ScrolledWindow::OnSize( wxSizeEvent &event )
{
    AdjustScrollBar();
    event.Skip();
}

void ScrolledWindow::DoPrepareDC( wxDC &dc )
{
    dc.SetDeviceOrigin( -GetViewStart().x, -GetViewStart().y );
}

void ScrolledWindow::OnPaint( wxPaintEvent &event )
{
    wxPaintDC dc(this);
    DoPrepareDC( dc );
    OnDraw( dc );
}

void ScrolledWindow::AdjustScrollBar()
{
    SetScrollbar(
        wxVERTICAL,
        GetViewStart().y,
        GetClientSize().GetHeight(),
        GetVirtualSize().GetHeight()
    );

    SetScrollbar(
        wxHORIZONTAL,
        GetViewStart().x,
        GetClientSize().GetWidth(),
        GetVirtualSize().GetWidth()
    );

    m_viewStart.y = GetScrollPos( wxVERTICAL );
    m_viewStart.x = GetScrollPos( wxHORIZONTAL );
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

void ScrolledWindow::DoGetViewStart( int *x, int *y ) const
{
    *x = m_viewStart.x;
    *y = m_viewStart.y;
}

void ScrolledWindow::Scroll( const wxPoint &pos )
    { Scroll( pos.x, pos.y ); }

void ScrolledWindow::Scroll( int x, int y )
{
    m_viewStart = wxPoint( x, y );
    SetScrollPos( wxHORIZONTAL, x );
    SetScrollPos( wxVERTICAL, y );
    AdjustScrollBar();
    Refresh();
}

void ScrolledWindow::Scroll( wxOrientation orient, int step )
    { DoScroll( orient, step ); }

void ScrolledWindow::DoScroll( wxOrientation orient, int step )
{
    int pos = GetScrollPos( orient ) + step;
    SetScrollPos( orient, pos );
    pos = GetScrollPos( orient );

    if ( orient == wxVERTICAL )
        Scroll( GetScrollPos( wxHORIZONTAL ), pos );

    if ( orient == wxHORIZONTAL )
        Scroll( pos, GetScrollPos( wxVERTICAL ) );
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
    wxDirection direction;
    wxOrientation orient;
    int pos = event.GetPosition();

    if ( event.GetOrientation() == wxVERTICAL )
        orient = wxVERTICAL;
    else if ( event.GetOrientation() == wxHORIZONTAL )
        orient = wxHORIZONTAL;

    int bottom_edge = GetScrollPos( orient );
    int virtual_edge = dimension::Get( GetVirtualSize(), orient ) - GetScrollThumb( orient );

    if ( pos < 0 && GetScrollPos( orient ) == 0 )
    {
        if ( orient == wxVERTICAL )
            direction = wxUP;
        if ( orient == wxHORIZONTAL )
            direction = wxLEFT;
    }
    else if
        (
            bottom_edge == virtual_edge
            && pos > bottom_edge
        )
    {
        if ( orient == wxVERTICAL )
            direction = wxDOWN;
        if( orient == wxHORIZONTAL )
            direction = wxRIGHT;
    }
    else
    {
        int step = pos - GetScrollPos( orient );
        Scroll( orient, step );
        event.Skip();
        return;
    }

    // this will inverse the direction
    // if m_isFromRight is true
    // and direction is left or right
    if ( m_isFromRight && (direction == wxLEFT || direction == wxRIGHT) )
        direction = ( direction == wxLEFT ) ?
            wxRIGHT : wxLEFT;

    OnEdge( direction );
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

    event.Skip();
}

void ScrolledWindow::OnMouseMotion( wxMouseEvent &event )
{
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

            wxSize virtual_size = GetVirtualSize();
            if ( virtual_size.GetWidth() > GetClientSize().GetWidth() )
                view.x -= view_x;
            if  ( virtual_size.GetHeight() > GetClientSize().GetHeight() )
                view.y -= view_y;

            Scroll( view );
        }
    }

    // store last mouse position
    // to track movement
    lastPos = event.GetPosition();
    event.Skip();
}

void ScrolledWindow::OnScrollThumbTrack( wxScrollWinEvent &event )
{
    dimension::Set( m_viewStart, event.GetOrientation(), event.GetPosition() );
    Refresh();
    // wxQueueEvent( this,
        // new wxScrollWinEvent(
            // wxEVT_SCROLLWIN_THUMBTRACK,
            // event.GetPosition(),
            // event.GetOrientation()
        // )
    // );
}

