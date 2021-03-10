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

#ifndef FMR_QUEUE_FIND_HANDLER_CHECKER
#define FMR_QUEUE_FIND_HANDLER_CHECKER

#include <fmr/handler/handler_factory.h>
#include <fmr/queue/find_handler_flags.h>

namespace fmr {

namespace queue {

class FindHandler;
class FindHandlerChecker {
 public:
  virtual FindStatus Check(FindHandler &parent,
                           AbstractOpenableHandler &handler, SStream &stream) {
    return Check(parent, static_cast<AbstractHandler &>(handler), stream);
  };
  virtual FindStatus Check(FindHandler &parent, AbstractHandler &handler,
                           SStream &stream) = 0;
};

}  // namespace queue

}  // namespace fmr

#endif /* end of include guard: FMR_QUEUE_FIND_HANDLER_CHECKER */
