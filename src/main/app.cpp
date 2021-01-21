/*
 *  Copyright (c) 2020-2021 Uskrai
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

#include <fmr/common/config.h>
#include <fmr/gui/frame.h>
#include <fmr/main/app.h>
#include <wx/display.h>
#include <wx/image.h>
#include <wx/intl.h>
#include <wx/log.h>
#include <wx/stdpaths.h>

#include <fstream>

wxIMPLEMENT_APP(fmr::App);

namespace fmr {

bool App::OnInit() {
  SetAppName(PACKAGE_NAME);
  wxInitAllImageHandlers();

  this->config = new Config(wxEmptyString, wxEmptyString,
                            wxStandardPaths::Get().GetUserDataDir());
  Config::Set(this->config);

  wxLocale locale;
  locale.Init(wxLANGUAGE_ENGLISH);

  log_stream =
      new std::fstream("log.txt", log_stream->binary | log_stream->app);
  wxLog::SetActiveTarget(new wxLogStream(log_stream));
  //
  try {
    std::locale::global(std::locale("en_US.UTF-8"));
  } catch (std::runtime_error &error) {
    wxLogMessage("Can't find user-preferred locale, defaulting to %s",
                 std::locale::classic().name());
    std::locale::global(std::locale::classic());
  }
  wxLogMessage("Opening program with %s locale", std::locale().name());

  std::setlocale(LC_ALL, "");
  std::locale::global(std::locale(std::locale::classic()));

  wxRect screen = wxDisplay().GetClientArea();
  App::frame = new Frame(PACKAGE_NAME, wxPoint(0, 0),
                         wxSize(screen.GetWidth(), screen.GetHeight()),
                         wxDEFAULT_FRAME_STYLE);
  App::frame->Maximize(true);
  App::frame->Show(true);
  return true;
}

int App::OnExit() {
  delete this->config;
  log_stream->close();
  delete log_stream;

  return 0;
}

};  // namespace fmr
