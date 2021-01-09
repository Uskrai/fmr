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

#include "gui/panel.h"
#include "base/path.h"
#include "base/config.h"
#include "handler/abstract_handler.h"

#include <wx/sizer.h>

namespace fmr
{

wxBEGIN_EVENT_TABLE( Panel, wxPanel )
    EVT_CHAR_HOOK( Panel::OnCharHook )
    EVT_COMMAND( wxID_ANY, EVT_OPEN_FILE, Panel::OnExplorerOpenFile )
wxEND_EVENT_TABLE()

Panel::Panel( wxWindow* parent, wxWindowID id, wxPoint position, wxSize size ) :
    wxPanel( parent, id, position, size )
{
    sizer_ = new wxBoxSizer( wxHORIZONTAL );
    reader_ = new reader::Window( this, ReaderWindow, wxDefaultPosition, GetClientSize(), 0, "Reader" );    
    if ( ! reader_->IsTransparentBackgroundSupported() )
        reader_->SetBackgroundColour( *wxBLACK );
    sizer_->Add( reader_, 1, wxALL | wxEXPAND );

    explorer_ = new explorer::Window(
        this,
        ExplorerWindow,
        wxDefaultPosition,
        GetClientSize(),
        0,
        "Explorer"
    );
    explorer_->SetBackgroundColour( *wxBLACK );
    explorer_->Hide();
    sizer_->Add( explorer_, 1, wxALL | wxEXPAND );

    SetSizer( sizer_ );
};

bool Panel::LoadFile( std::string path )
{
    bool ret = reader_->Open( path );
    reader_->Show();
    if ( ret )
        reader_->SetFocus();

    if ( ret && explorer_ )
    {
        explorer_->Hide();
        explorer_->Clear();
    }


    return ret;
}

bool Panel::OpenExplorer()
{
    std::string path, select_path;

    if ( reader_ )
    {
        auto handler = reader_->GetHandler();
        if ( handler )
        {
            select_path = handler->GetName();

            if ( handler->GetParent() )
                path = handler->GetParent()->GetName();
            else
                path = select_path;
        }
    }

    if ( path == "" )
    {
        select_path = Config::Get()->Read("RecentlyOpened", wxString() );
        path = Path::GetParent( select_path );
    }

    if ( ! explorer_ )
        return false;

    if ( explorer_->Open( path ) )
    {
        explorer_->Show();
        explorer_->SetFocus();
        explorer_->Select( select_path );

        if ( reader_ )
        {
            reader_->Clear();
            reader_->Hide();
        }

        sizer_->Layout();

        return true;
    }

    return false;
}

void Panel::OnCharHook( wxKeyEvent &event )
{
    if ( event.GetKeyCode() == WXK_BACK )
        return void( OpenExplorer() );
    event.DoAllowNextEvent();
    event.Skip();
}

void Panel::OnExplorerOpenFile( wxCommandEvent &event )
{
    explorer_->Hide();
    if ( LoadFile( String::ToString( event.GetString() ) ) )
    {
        explorer_->Clear();
    }
    else
        explorer_->Show();
}

bool Panel::Destroy()
{
    if ( reader_ )
        reader_->Destroy();
    if ( explorer_ )
        explorer_->Destroy();
    return wxWindow::Destroy();
}

}; // namespace fmr