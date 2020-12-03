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
        m_vScrollBar = new wxScrollBar(this,wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSB_VERTICAL );
    if ( !m_hScrollBar )
        m_hScrollBar = new wxScrollBar(this,wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSB_HORIZONTAL );
    
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

void ScrolledWindow::Scroll( int x, int y )
{
    m_vScrollBar->SetThumbPosition( y );
    m_viewStart.y = m_vScrollBar->GetThumbPosition();
    m_hScrollBar->SetThumbPosition( x );
    m_viewStart.x = m_hScrollBar->GetThumbPosition();
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
    Scroll(0,1000);
    m_virtualSize = wxSize(width,height);
    AdjustScrollBar();
}

