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

#include "app.h"
#include "gui/frame.h"
#include "base/config.h"

#include <wx/display.h>
#include <wx/image.h>

#include <wx/stdpaths.h>
#include <clocale>

wxIMPLEMENT_APP(fmr::App);

namespace fmr
{


bool App::OnInit()
{
    SetAppName(PACKAGE_NAME);
    wxInitAllImageHandlers();

    this->config =  new Config
                    (  
                        wxEmptyString,
                        wxEmptyString,
                        wxStandardPaths::Get().GetUserDataDir()
                    );
    Config::Set ( this->config );
    std::locale::global( std::locale("") );
    wxRect screen = wxDisplay().GetClientArea();
    App::frame = new Frame(PACKAGE_NAME,wxPoint(0,0),wxSize(screen.GetWidth(),screen.GetHeight()),wxDEFAULT_FRAME_STYLE);
    App::frame->Maximize(true);
    App::frame->Show(true);
    return true;
}

int App::OnExit()
{
    delete this->config;
    return 0;
}

}; // namespace fmr