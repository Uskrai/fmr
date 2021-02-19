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
#ifndef FMR_COMMON_DIMENSION
#define FMR_COMMON_DIMENSION

#include <fmr/common/bitmask.h>
#include <wx/gdicmn.h>
#include <wx/position.h>

#include "wx/defs.h"

namespace dimension {

enum Orientation { kVertical = 0x01, kHorizontal = 0x02 };

inline int Get(const wxPoint &pos, const wxOrientation &orient) {
  if (orient == wxVERTICAL)
    return pos.y;
  else if (orient == wxHORIZONTAL)
    return pos.x;

  return wxDefaultCoord;
}

inline void Set(wxPoint &pt, int orientation, int pos) {
  if (orientation == wxVERTICAL) pt.y = pos;
  if (orientation == wxHORIZONTAL) pt.x = pos;
}

inline void SetPoint(wxPoint &pt, Orientation orientation, int pos) {
  if (orientation == kHorizontal) pt.y = pos;
  if (orientation == kHorizontal) pt.x = pos;
}

template <typename T>
inline void SetPoint(T &pt, Orientation orientation, int pos) {
  if (orientation == kVertical) pt.SetY(pos);
  if (orientation == kHorizontal) pt.SetX(pos);
}

inline void SetSize(wxSize &size, Orientation orient, int val) {
  if (orient == kVertical) size.SetHeight(val);
  if (orient == kHorizontal) size.SetWidth(val);
};

template <typename T>
inline void SetSize(T &size, Orientation orient, int val) {
  if (orient == kVertical) size.SetHeight(val);
  if (orient == kHorizontal) size.SetWidth(val);
}

inline int GetPoint(const wxPoint &pt, Orientation orient) {
  if (orient == kVertical) return pt.y;
  if (orient == kHorizontal) return pt.x;
  return wxDefaultCoord;
}

template <typename T>
inline int GetPoint(const T &pt, Orientation orient) {
  if (orient == kVertical) return pt.GetHeight();
  if (orient == kHorizontal) return pt.GetWidth();
  return wxDefaultCoord;
}

template <typename T>
inline int GetSize(const T &size, Orientation orient) {
  if (orient == kVertical) return size.GetHeight();
  if (orient == kHorizontal) return size.GetWidth();
  return wxDefaultCoord;
}

inline int Get(const wxSize &size, const wxOrientation &orient) {
  wxPoint pos(size.GetWidth(), size.GetHeight());
  return Get(pos, orient);
}

template <class T>
inline void print(const T &dimension, const char *text) {
  printf("%s %dx%d\n", text, dimension.x, dimension.y);
}

inline void print(const char *text, wxRect rect) {
  printf("%s pos:%dx%d\tsize:%dx%d\n", text, rect.x, rect.y, rect.width,
         rect.height);
}

inline wxDirection GetDirection(const wxOrientation &orient, int pos) {
  if (pos == 0) return wxALL;

  if (orient == wxVERTICAL)
    return pos < 0 ? wxUP : wxDOWN;
  else if (orient == wxHORIZONTAL)
    return pos < 0 ? wxLEFT : wxRIGHT;

  return wxALL;
}

inline wxOrientation GetOrient(const int &orient) {
  if (orient == wxVERTICAL) return wxVERTICAL;
  if (orient == wxHORIZONTAL) return wxHORIZONTAL;
  return wxORIENTATION_MASK;
}

/**
 * @brief: return orientation from wxDirection
 *
 * @param: direction to check the orientation
 *
 * @return: will return the orientation respectively
 * will return wxBOTH if not found ( not up,down,left, or right )
 */
inline wxOrientation GetOrient(const wxDirection &direction) {
  if (direction == wxUP || direction == wxDOWN) return wxVERTICAL;

  if (direction == wxLEFT || direction == wxRIGHT) return wxHORIZONTAL;

  return wxBOTH;
}

inline wxPoint Make(const wxPoint &pt, wxOrientation orient, int pos) {
  wxPoint temp(pt);
  dimension::Set(temp, orient, pos);
  return temp;
}

/**
 * @brief: return Direction from wxKeyEvent
 *
 * @param: key_code return value from wxKeyEvent.GetKeyCode()
 *
 * @return: return the respective Direction
 * will return wxALL if not found
 */
inline wxDirection GetDirection(int key_code) {
  if (key_code == WXK_UP)
    return wxUP;
  else if (key_code == WXK_DOWN)
    return wxDOWN;
  else if (key_code == WXK_LEFT)
    return wxLEFT;
  else if (key_code == WXK_RIGHT)
    return wxRIGHT;
  return wxALL;
}

inline wxPoint AlignPosition(const wxRect &rect, const wxSize &size,
                             int flags) {
  wxPoint pos;

  if (flags & wxALIGN_LEFT) {
    pos.x = rect.GetLeft();
  } else if (flags & wxALIGN_RIGHT) {
    pos.x = rect.GetRight() - size.GetWidth();
  } else if (flags & wxALIGN_CENTER_HORIZONTAL) {
    pos.x = rect.GetWidth() / 2 - size.GetHeight() / 2;
  }
  if (flags & wxALIGN_TOP)
    pos.y = rect.GetTop();
  else if (flags & wxALIGN_BOTTOM)
    pos.y = rect.GetBottom() - size.GetHeight();
  else if (flags & wxALIGN_CENTER_VERTICAL)
    pos.y = rect.GetHeight() / 2 - size.GetHeight() / 2;

  return pos;
}

}  // namespace dimension

#endif
