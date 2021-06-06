/*
 *  Copyright (c) 2021 Uskrai
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

#ifndef INCLUDE_FMR_NWD_FS_H_
#define INCLUDE_FMR_NWD_FS_H_

#ifdef FMR_OPTION_USE_STD_FILESYSTEM
#include <filesystem>
#else
#include <boost/filesystem.hpp>
#endif

namespace fmr {

namespace nwd {

#ifdef FMR_OPTION_USE_STD_FILESYSTEM
namespace fs = std::filesystem;
#else
namespace fs = boost::filesystem;
#endif

}  // namespace nwd

}  // namespace fmr

#endif  // INCLUDE_FMR_NWD_FS_H_
