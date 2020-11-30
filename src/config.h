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

#include <wx/fileconf.h>

class Config :
    private wxFileConfig
{
        static Config* global;
    public:
        Config() : wxFileConfig( "fmr", "uskrai", "config.ini", "config.ini" ) {};
        template<typename T>
        T Read( wxString name, T defaultValue )
        {
            if ( wxFileConfig::Read( name, &defaultValue )
            {
                return defaultValue;
            }
            else
            {
                wxFileConfig::Write(name, defaultValue);
                return defaultValue;
            }
        }
        template<typename T>
        bool Write( const wxString name, const T value ) 
        {
            return wxFileConfig::Write(name,value);
        }
        static void Set( Config* global ) { Config::global = global; }
        static Config* Get() { return Config::global; }
};