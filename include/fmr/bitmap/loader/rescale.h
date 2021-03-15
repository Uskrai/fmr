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

#ifndef FMR_BITMAP_LOADER_RESCALE
#define FMR_BITMAP_LOADER_RESCALE

#include <fmr/bitmap/loader/base.h>
#include <fmr/bitmap/loader/rescale_container.h>

namespace fmr {

namespace bitmap {

namespace loader {

class RescaleContainer;

class Rescale : public Base {
  using RescaleQueueData =
      thread::QueueThreadData<queue::RescaleItem, queue::Rescale,
                              RescaleReceiverEvent>;
  using RescaleQueueCtrl = thread::QueueThreadCtrl<queue::RescaleItem>;

  std::unique_ptr<RescaleQueueData> rescale_data_;

 public:
  Rescale(int event_id);
  virtual ~Rescale();

  virtual RescaleContainer *GetContainer() override;
  virtual const RescaleContainer *GetContainer() const override;

  void SetRescaler(Rescaler *rescaler);

  void PushRescale(const SStream *found_stream, const wxImage &img);

  void Clear();

 protected:
  virtual void OnImageLoaded(ImageLoadEvent &event) override;
  virtual void OnImageRescaled(RescaleEvent &event);

  virtual void SetContainer(std::unique_ptr<RescaleContainer> container);
};

}  // namespace loader

}  // namespace bitmap

}  // namespace fmr

#endif /* end of include guard: FMR_BITMAP_LOADER_RESCALE */
