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

#ifndef FMR_COMMON_CONFIG
#define FMR_COMMON_CONFIG

#include <wx/fileconf.h>

namespace fmr {

class Config : private wxFileConfig {
 private:
 public:
  using wxFileConfig::GetPath;
  using wxFileConfig::wxFileConfig;

  wxString Read(wxString name, const char* defaultValue) {
    wxString string = wxString(defaultValue);
    return this->Read(name, string);
  }

  template <typename T>
  T Read(wxString name, T defaultValue) {
    if (wxFileConfig::Read(name, &defaultValue)) {
      return defaultValue;
    } else {
      this->Write(name, defaultValue);
      return defaultValue;
    }
  }

  using wxFileConfig::Flush;
  using wxFileConfig::Write;

  static Config* Get() { return static_cast<Config*>(wxFileConfig::Get()); }

  static void Set(Config* pConfig) { wxFileConfig::Set(pConfig); }
};

}  // namespace fmr

#endif
