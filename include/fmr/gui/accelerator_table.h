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

#include <wx/accel.h>

#include <vector>

namespace fmr {

namespace gui {

class AcceleratorTable {
  std::vector<wxAcceleratorEntry> vec_entry_;

 public:
  void AddEntry(int flags = 0, int key_code = 0, int command = 0,
                wxMenuItem *item = nullptr);

  wxAcceleratorTable GetTable() const;
};

}  // namespace gui

}  // namespace fmr
