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

#ifndef FMR_COMMON_CONFIG
#define FMR_COMMON_CONFIG

#include <wx/fileconf.h>

namespace fmr
{

class Config :
    private wxFileConfig
{
    private:
        inline static Config* global = NULL;
    public:
        using wxFileConfig::wxFileConfig;
        using wxFileConfig::GetPath;

        
        wxString Read( wxString name, const char* defaultValue )
        {
            wxString string = wxString(defaultValue);
            return this->Read( name, string );
        }

        template<typename T>
        T Read( wxString name, T defaultValue )
        {
            if ( wxFileConfig::Read( name, &defaultValue ) )
            {
                return defaultValue;
            }
            else
            {
                this->Write(name, defaultValue);
                return defaultValue;
            }
        }

        using wxFileConfig::Write;
        using wxFileConfig::Flush;

        static Config* Get() { return Config::global; }

        static void Set( Config* pConfig ) { Config::global = pConfig; }
};

};

#endif
