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

#ifndef FMR_BITMAP_PAGE_LOADER
#define FMR_BITMAP_PAGE_LOADER

#include <wx/event.h>

#include <stack>

#include "fmr/bitmap/bitmap_page_ctrl.h"
#include "fmr/bitmap/loader.h"

namespace fmr {

namespace bitmap {

wxDECLARE_EVENT(kEventOpenedStreamFound, wxCommandEvent);

class PageLoader : public Loader {
  std::stack<SStream *> stream_before_opened_;  // queue for stream that is
                                                // found before the opened path
  std::unique_ptr<AbstractHandler> handler_;
  std::vector<std::vector<SStream *>> per_page_stream_;
  BitmapPageCtrl *bmp_ctrl_ = nullptr;

  size_t opened_index_ = -1, opened_page_ = -1;
  const SStream *opened_stream_ = nullptr;
  size_t image_per_page_ = -1;

 public:
  PageLoader(wxEvtHandler *handler, BitmapPageCtrl *bmp_ctrl, int id);
  virtual ~PageLoader() { Clear(); }

  bool Open(const std::string &path);
  AbstractHandler *GetHandler() { return handler_.get(); }

  void SetImagePerPage(size_t size) { image_per_page_ = size; }
  size_t GetImagePerPage() const { return image_per_page_; }
  size_t GetStreamPage(const SStream *stream);
  size_t GetStreamPosInPage(const SStream *stream);

  size_t GetOpenedPagePos() const { return opened_page_; }

  BitmapPageCtrl *GetBitmapCtrl() { return bmp_ctrl_; }

  virtual void Clear();

  size_t CountLoadedImage();

  virtual void SetControllerId(int find_controller_id,
                               int load_image_controller_id) override;

 protected:
  bool IsFoundStreamInBack(const SStream *found_stream);
  void PushStreamStack();
  void SendOpenedPageInBack();
  size_t SourceStreamToIndex(const SStream *stream);
  const SStream *GetFirstFoundStream(const SStream *source_stream);

 private:
  void OnStreamFound(thread::FoundEvent &event);
  void OnCompleted(wxThreadEvent &event);
};

}  // namespace bitmap

}  // namespace fmr

#endif /* end of include guard: FMR_BITMAP_PAGE_LOADER */
