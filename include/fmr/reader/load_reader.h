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

#ifndef FMR_READER_LOADTHREAD
#define FMR_READER_LOADTHREAD

#include <fmr/bitmap/bitmap.h>
#include <fmr/bitmap/bmp.h>
#include <fmr/handler/abstract_handler.h>
#include <fmr/handler/struct_stream.h>
#include <fmr/thread/thread.h>
#include <wx/string.h>

#include <memory>

class ScrolledWindow;
class Bitmap;

namespace fmr {

namespace reader {

inline wxCriticalSection LoadThreadLock;
class LoadThread : public BaseThread {
 public:
  LoadThread(ThreadController *parent,
             const wxThreadKind type = wxTHREAD_DETACHED, int id = -1)
      : BaseThread(parent, type, id){};
  void SetParameter(std::shared_ptr<AbstractHandler> handler,
                    std::shared_ptr<Bitmap> bitmap, size_t start);
  void SetHandler(std::shared_ptr<AbstractHandler> handler);
  void SetBitmap(std::shared_ptr<Bitmap> bitmap);
  void SetStart(size_t start);
  void SetLimit(size_t prev, size_t next);

  std::shared_ptr<AbstractHandler> GetHandler() { return file_handler_; }
  std::shared_ptr<Bitmap> GetBitmap() { return bitmap_; }

  wxThreadError Run();

  wxCriticalSection &GetLock() { return LoadThreadLock; }
  static void LoadImage(std::shared_ptr<Bitmap> bmp,
                        std::shared_ptr<AbstractHandler> handler, size_t idx,
                        wxCriticalSection &lock);
  static void LoadImage(std::shared_ptr<Bitmap> bmp, SStream &stream,
                        size_t idx);

 protected:
  size_t load_start_;
  std::shared_ptr<Bitmap> bitmap_;
  std::shared_ptr<AbstractHandler> file_handler_;
  virtual ExitCode Entry();
  void LoadImage(size_t &idx, int step);
};

};  // namespace reader

};  // namespace fmr
#endif
