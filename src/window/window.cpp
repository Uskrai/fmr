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
    EVT_KEY_DOWN( ScrolledWindow::OnKeyDown )
    EVT_MOUSEWHEEL( ScrolledWindow::OnMouseWheel )
    EVT_MOTION( ScrolledWindow::OnMouseMotion )
    EVT_SCROLLWIN( ScrolledWindow::OnScroll )
wxEND_EVENT_TABLE()

ScrolledWindow::ScrolledWindow( wxWindow *parent, 
                                wxWindowID id,
                                const wxPoint &pos,
                                const wxSize &size,
                                long style,
                                const wxString &name
                            )
{
    Create( parent, id, pos, size, style, name);
}

ScrolledWindow::~ScrolledWindow()
{
    m_vScrollBar->Destroy();
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
    bool ret = wxWindow::Create( parent, id, pos, size, style, name );
    if ( !m_vScrollBar ) 
        m_vScrollBar = new wxScrollBar(this, VerticalScrollBar , wxDefaultPosition, wxDefaultSize, wxSB_VERTICAL );
    if ( !m_hScrollBar )
        m_hScrollBar = new wxScrollBar(this, HorizontalScrollBar , wxDefaultPosition, wxDefaultSize, wxSB_HORIZONTAL );
    
    SetVirtualSize(0,0);
    return ret;
}

void ScrolledWindow::OnSize( wxSizeEvent &event )
{
    AdjustScrollBar();
    event.Skip();
}

void ScrolledWindow::DoPrepareDC( wxDC &dc )
{
    dc.SetDeviceOrigin( m_viewStart.x, -m_viewStart.y );
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
    if ( step == 0 )
        return;

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

    wxEventType type;
    if ( step > 0 )
        type = wxEVT_SCROLLWIN_LINEDOWN;
    else if ( step < 0 )
        type = wxEVT_SCROLLWIN_LINEUP;

    wxQueueEvent( this, new wxScrollWinEvent( type, pos, orient ) );
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
        wxOrientation orient;
        int modifier = 0;

        // set orientation to vertical
        // if key is up or down
        if ( isUp || isDown )
            orient = wxVERTICAL;
        
        // set orientation to horizontal
        // if key is left or right
        if ( isLeft || isRight )
            orient = wxHORIZONTAL;
        
        // set modifier to -1 
        // if key is up or right
        // negative to substract
        // viewScroll
        if ( isUp || isRight )
            modifier = -1;
        
        if ( isDown || isLeft )
            modifier = 1;

        Scroll(orient,modifier * m_stepPerKey );
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

    Scroll( orient, step * m_stepPerWheel );

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

void ScrolledWindow::OnScroll( wxScrollWinEvent &event )
{

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

    m_vScrollBar->SetScrollbar(
        m_vScrollBar->GetThumbPosition(), 
        sz.GetHeight(), 
        GetVirtualSize().GetHeight(), 
        sz.GetHeight()
    );

    m_hScrollBar->SetScrollbar(
        m_hScrollBar->GetThumbPosition(), 
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

