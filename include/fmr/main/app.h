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

#include <fmr/file_handler/factory.h>
#include <wx/app.h>

#include <iostream>
#include <memory>

class wxFrame;

namespace fmr {

class Config;

class App : public wxApp {
  wxFrame* frame;
  std::unique_ptr<file_handler::Factory> file_handler_factory_;
  std::unique_ptr<Config> config_;
  std::ofstream* log_stream;
  bool OnInit();
  int OnExit();

  void PrepareConfig();
  void PrepareFileHandlerFactory();

 public:
  void OpenFile(wxString Path);
};

}  // namespace fmr
