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

#ifndef FMR_BITMAP_LOADER_PAGE
#define FMR_BITMAP_LOADER_PAGE

#include <fmr/bitmap/loader/base.h>
#include <fmr/handler/handler_factory.h>

namespace fmr {

namespace bitmap {

class BitmapPageCtrl;

namespace loader {

wxDECLARE_EVENT(kEventOpenedStreamFound, wxCommandEvent);

class Page : public Base {
  BitmapPageCtrl *bmp_ctrl_;
  std::unique_ptr<AbstractHandler> handler_;
  size_t opened_index_ = -1, item_per_page_ = 1;
  size_t preload_prev_ = 1, preload_next_ = 1;
  SStream *opened_stream_ = nullptr;

  std::vector<std::vector<SStream *>> stream_page_;

 public:
  Page(int event_id, BitmapPageCtrl *bmp_ctrl);
  virtual ~Page();

  bool Open(const std::string &path);
  AbstractHandler *GetHandler();
  const AbstractHandler *GetHandler() const;

  void AdjustLoad();
  void PreloadImage();
  void ClearUnused();

  bool ItemExist(size_t page_pos, size_t img_pos);
  bool ShouldLoad(size_t page_pos, size_t img_pos, bool make_front);

  void ClearPage(size_t page_pos);
  void LoadImageIfNeeded(size_t page_pos, size_t img_pos, bool make_front);
  void LoadPageIfNeeded(size_t page_pos, bool make_front);

  void SetPreloadPrev(size_t size) { preload_prev_ = size; }
  void SetPreloadNext(size_t size) { preload_next_ = size; }

  void SetItemPerPage(size_t size) { item_per_page_ = size; }
  size_t GetItemPerPage() const { return item_per_page_; }

  size_t GetStreamPage(const SStream *stream) const;
  size_t GetStreamPosInPage(const SStream *stream) const;

  void SendOpenedStreamInBack();
  size_t GetOpenedPagePos() { return opened_index_; }

  void Clear();

 protected:
  virtual void OnFindItemPush(FindEvent &event) override;

  virtual void OnBitmapChanged(BitmapVectorEvent &event);
};

}  // namespace loader

}  // namespace bitmap

}  // namespace fmr

#endif /* end of include guard: FMR_BITMAP_LOADER_PAGE */
