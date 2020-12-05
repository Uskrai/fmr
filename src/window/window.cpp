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

#include "window/window.h"
#include <wx/scrolbar.h>
#include <wx/dc.h>
#include <wx/dcclient.h>
#include <wx/event.h>

wxBEGIN_EVENT_TABLE( ScrolledWindow, wxWindow )
    EVT_SIZE( ScrolledWindow::OnSize )
    EVT_PAINT( ScrolledWindow::OnPaint )
#ifdef __WXMSW__
    EVT_CHAR_HOOK( ScrolledWindow::OnKeyDown )
#else
    EVT_KEY_DOWN( ScrolledWindow::OnKeyDown )
#endif
    EVT_MOUSEWHEEL( ScrolledWindow::OnMouseWheel )
    EVT_MOTION( ScrolledWindow::OnMouseMotion )
    EVT_SCROLLWIN_LINEUP( ScrolledWindow::OnScrollLine )
    EVT_SCROLLWIN_LINEDOWN ( ScrolledWindow::OnScrollLine )
    EVT_COMMAND_SCROLL_THUMBTRACK( VerticalScrollBar, ScrolledWindow::OnScrollThumbTrack )
    EVT_COMMAND_SCROLL_THUMBTRACK( HorizontalScrollBar, ScrolledWindow::OnScrollThumbTrack )
wxEND_EVENT_TABLE()

ScrolledWindow::ScrolledWindow( wxWindow *parent, 
                                wxWindowID id,
                                const wxPoint &pos,
                                const wxSize &size,
                                long style,
                                const wxString &name
                            )
    : wxWindow( parent, id , pos, size, style, name )
{
    CreateScrollBar( wxBOTH );
    SetVirtualSize(0,0);
}

ScrolledWindow::~ScrolledWindow()
{
    if ( m_vScrollBar )
        m_vScrollBar->Destroy();
    if ( m_hScrollBar )
        m_hScrollBar->Destroy();
}

bool ScrolledWindow::Create(    wxWindow *parent, 
                                wxWindowID id, 
                                const wxPoint &pos,
                                const wxSize &size,
                                long style,
                                const wxString &name
                            ) 
{
    bool ret = wxWindow::Create( parent, id,  pos, size, style, name );
    CreateScrollBar( wxBOTH );
    SetVirtualSize(0,0);
    return ret;
}

void ScrolledWindow::CreateScrollBar( wxOrientation orient )
{
    if ( ( orient & wxVERTICAL ) && ! m_vScrollBar )
        m_vScrollBar = new wxScrollBar( this, VerticalScrollBar, wxDefaultPosition, wxDefaultSize, wxSB_VERTICAL );
    
    if ( ( orient & wxHORIZONTAL ) && ! m_hScrollBar )
        m_hScrollBar = new wxScrollBar( this, HorizontalScrollBar, wxDefaultPosition, wxDefaultSize, wxSB_HORIZONTAL );
}

void ScrolledWindow::OnSize( wxSizeEvent &event )
{
    AdjustScrollBar();
    event.Skip();
}

void ScrolledWindow::DoPrepareDC( wxDC &dc )
{
    dc.SetDeviceOrigin( -m_viewStart.x, -m_viewStart.y );
}

void ScrolledWindow::OnPaint( wxPaintEvent &event )
{
    wxPaintDC dc(this);
    DoPrepareDC( dc );
    OnDraw( dc );
}

void ScrolledWindow::Scroll( const wxPoint &pos )
{
    Scroll( pos.x, pos.y );
}

int GetScrollBarBottom( wxScrollBar *bar )
{
    return bar->GetRange() - bar->GetThumbSize();
    
}

void ScrolledWindow::DoScroll( wxScrollBar *scrollbar, int step )
{
    scrollbar->SetThumbPosition( 
        scrollbar->GetThumbPosition() + step
    );
}

void ScrolledWindow::DoScroll( wxOrientation orient, int step )
{
    int pos;
    if ( orient == wxVERTICAL )
    {
        DoScroll( m_vScrollBar, step );
        pos = m_vScrollBar->GetThumbPosition();
        m_viewStart.y = pos;
    }

    if ( orient == wxHORIZONTAL )
    {
        DoScroll( m_hScrollBar, step );
        pos = m_hScrollBar->GetThumbPosition();
        m_viewStart.x = pos;
    }
}

void ScrolledWindow::Scroll( int x, int y )
{
    DoScroll( wxHORIZONTAL, x - GetViewStart().x );
    DoScroll( wxVERTICAL, y - GetViewStart().y );
    Refresh();
}

void ScrolledWindow::Scroll( wxOrientation orient, int step )
{
    DoScroll( orient, step );
    Refresh();
}

void ScrolledWindow::DoScrollLine( wxEventType type, wxOrientation orient, int step )
{    
    int pos = GetScrollPos( orient );
    Scroll( orient, step );
    pos = ( pos == GetScrollPos( orient ) ) ?
        pos + step : GetScrollPos( orient );

    wxQueueEvent(
        this,
        new wxScrollWinEvent(
            type,
            pos,
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
    int orient = event.GetOrientation();
    int pos = event.GetPosition();
    wxScrollBar *bar;

    if ( orient == wxVERTICAL )
        bar = m_vScrollBar;
    else if ( orient == wxHORIZONTAL )
        bar = m_hScrollBar;
    
    if ( pos < 0 )
    {
        if ( orient == wxVERTICAL )
            direction = wxUP;
        if ( orient == wxHORIZONTAL )
            direction = wxLEFT;
    }
    else if ( pos > bar->GetRange() - bar->GetThumbSize() )
    {
        if ( orient == wxVERTICAL )
            direction = wxDOWN;
        if( orient == wxHORIZONTAL )
            direction = wxRIGHT;
    }
    else 
        return;
    
    // this will inverse the direction
    // if m_isFromRight is true
    // and direction is left or right
    if ( m_isFromRight && (direction == wxLEFT || direction == wxRIGHT) )
        direction = ( direction == wxLEFT ) ?
            wxRIGHT : wxLEFT;
        
    
    OnEdge( direction );
}

int ScrolledWindow::GetScrollPos( wxOrientation orient )
{
     if ( orient == wxVERTICAL )
        return m_viewStart.y;
    else if ( orient == wxHORIZONTAL )
        return m_viewStart.x;
    return 0;
}

void ScrolledWindow::OnKeyDown( wxKeyEvent &event )
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
    int temp = GetScrollPos( orient );
    step *= ( m_isMouseWheelInvert ) ? -1 : 1;
    
    Scroll( orient, step * m_stepPerWheel );
    
    if ( temp == GetScrollPos( orient ) && step != 0 )
    {
        if ( orient == wxHORIZONTAL )
        {
            step *= ( m_isFromRight ) ?
                -1 : 1;

            if ( step > 0 )
                OnEdge( wxRIGHT );
            else if ( step < 0 )
                OnEdge( wxLEFT );
        }
        if ( orient == wxVERTICAL )
        {
            if ( step > 0 )
                OnEdge( wxDOWN );
            else if ( step < 0 )
                OnEdge( wxUP );
        }
    }


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
        if ( event.Dragging() )
        {
            wxPoint view = GetViewStart();
            view.x -= event.GetX() - lastPos.x;
            view.y -= event.GetY() - lastPos.y;
            Scroll( view );
        }
    }

    // store last mouse position
    // to track movement
    lastPos = event.GetPosition();
    event.Skip();
}

void ScrolledWindow::OnScrollThumbTrack( wxScrollEvent &event )
{
    if ( event.GetOrientation() == wxHORIZONTAL )
        m_viewStart.x = event.GetPosition();
    else if ( event.GetOrientation() == wxVERTICAL )
        m_viewStart.y = event.GetPosition();

    Refresh();
    wxQueueEvent( this, 
        new wxScrollWinEvent( 
            wxEVT_SCROLLWIN_THUMBTRACK,
            event.GetPosition(),
            event.GetOrientation()
        )
    );
}


void ScrolledWindow::AdjustScrollBar()
{
    wxSize sz = GetClientSize();
    m_vScrollBar->SetPosition( wxPoint(
        sz.GetWidth() - m_vScrollBar->GetSize().GetWidth(), 
        0
    ));
    m_hScrollBar->SetPosition( wxPoint(
        0, 
        sz.GetHeight() - m_hScrollBar->GetSize().GetHeight()
    ));

    m_vScrollBar->SetSize( 
        -1,
        sz.GetHeight()
    );

    m_hScrollBar->SetSize(
        sz.GetWidth(),
        -1
    );

    m_viewStart.y = m_vScrollBar->GetThumbPosition();
    m_vScrollBar->SetScrollbar(
        GetViewStart().y,
        sz.GetHeight(), 
        GetVirtualSize().GetHeight(), 
        sz.GetHeight()
    );

    m_viewStart.x = m_hScrollBar->GetThumbPosition();
    m_hScrollBar->SetScrollbar(
        GetViewStart().x,
        sz.GetWidth(), 
        GetVirtualSize().GetWidth(), 
        sz.GetWidth()
    );

    if ( m_hScrollBar->GetRange() <= GetClientSize().GetWidth() )
        m_hScrollBar->HideWithEffect( wxSHOW_EFFECT_BLEND );
    else 
        m_hScrollBar->Show();
    if ( m_vScrollBar->GetRange() <= GetClientSize().GetHeight() )
        m_vScrollBar->HideWithEffect( wxSHOW_EFFECT_BLEND );
    else
        m_vScrollBar->Show();
}

void ScrolledWindow::DoSetVirtualSize( int width, int height )
{
    m_virtualSize = wxSize(width,height);
    AdjustScrollBar();
}

