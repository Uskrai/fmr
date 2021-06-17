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

#ifndef INCLUDE_FMR_WX_ARCHIVE_PROVIDER_H_
#define INCLUDE_FMR_WX_ARCHIVE_PROVIDER_H_

#include <fmr/find/checker.h>
#include <wx/stream.h>

class wxArchiveEntry;

namespace fmr {

namespace wx {

namespace archive {

/**
 * This class is an abstract class used to interact with fmr::wx::archive::Find
 */
class Provider {
 public:
  virtual ~Provider() {}
  /**
   * Consume Searched entry from Non Seekable Archive Stream
   *
   * Stream consumed with this method is not sorted
   *
   * see:
   * https://docs.wxwidgets.org/3.0.4/overview_archive.html#overview_archive_noseek
   *
   * \param entry archive entry
   * \param stream Stream that contain data of entry
   */
  virtual void ConsumeNonSeekable(wxArchiveEntry &entry,
                                  wxInputStream &stream) = 0;

  /**
   * Consume Searched Entry from Seekable Archive Stream
   *
   * \param entry archive entry
   * \param stream Stream that contain data of entry
   */
  virtual void ConsumeSeekable(wxArchiveEntry &entry,
                               wxInputStream &stream) = 0;
};

}  // namespace archive

}  // namespace wx

}  // namespace fmr

#endif  // INCLUDE_FMR_WX_ARCHIVE_PROVIDER_H_
