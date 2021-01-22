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

#ifndef FMR_BITMAP_BITMAP
#define FMR_BITMAP_BITMAP

#include <fmr/bitmap/bmp.h>
#include <fmr/common/vector.h>

class wxWindow;

namespace fmr {

class Bitmap {
 public:
  Bitmap(wxWindow* parent);
  ~Bitmap();
  float Prepare(wxImage& image);
  void Add(wxImage& image, const size_t& idx, const float& scale);

  void SetName(size_t idx, wxString name) { GetAll()[idx].SetName(name); }

  void SetFlags(int posFlags, int sizeFlags, int scaleParent) {
    SetPosition(posFlags);
    SetSize(sizeFlags), SetScaleParent(scaleParent);
  }
  void SetScaleParent(int scale) { m_scaleParent = scale; }
  void SetSize(int flags) { m_flagSize = flags; }
  void SetPosition(int flags) { m_flagPosition = flags; }

  void SetLimit(size_t limit);
  void Resize(size_t limit);
  void Clear();

  void Refresh();
  void RefreshPosition(wxSize size);
  wxSize GetSize(wxSize min);

  BitmapPageChangeStatus ChangePage(int step);
  bool IsImageOk(int pos);
  void MarkLoaded(size_t idx);
  bool NextPage();
  bool PrevPage();

  wxVector<SBitmap>& GetAll() { return m_item; }

  wxVector<SBitmap*>& Get() { return m_itemPage; }

  SBitmap* Get(const wxPoint& area, const wxPoint& position);

  wxVector<SBitmap*> Get(const wxPoint& area, const wxSize& size);

 protected:
  wxWindow* m_parent = NULL;
  wxVector<SBitmap> m_item;
  wxVector<SBitmap*> m_itemPage;

 private:
  size_t m_limit = 1;
  size_t m_posFirst = 0, m_posLast = 0;

  int m_flagSize, m_flagPosition, m_scaleParent;
  int m_maxWidth, m_maxHeight;

  wxWindow* GetParent() { return m_parent; }
  void Exit(int i);

  int Push(struct SBitmap& bmp);

  int Centered(int width);
};

};  // namespace fmr

#endif
