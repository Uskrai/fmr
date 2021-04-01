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

#ifndef H_FMR_LOG_STOPWATCH_H
#define H_FMR_LOG_STOPWATCH_H

#include <fmt/chrono.h>
#include <fmt/format.h>

#include <chrono>

namespace fmr {

namespace log {

template <typename Duration = std::chrono::seconds>
class StopWatch {
 public:
  StopWatch() { Reset(); }

  using clock = std::chrono::steady_clock;
  using duration = Duration;
  std::chrono::time_point<clock> start_;
  using rep = typename duration::rep;

  template <typename Dur, typename Time>
  auto Cast(Time t) const {
    return std::chrono::duration_cast<Dur>(t);
  }

  duration Elapsed() const {
    return duration(Cast<duration>(clock::now() - start_));
  }

  void Reset() { start_ = clock::now(); }
};

using StopWatchNano = StopWatch<std::chrono::nanoseconds>;
using StopWatchMicro = StopWatch<std::chrono::microseconds>;
using StopWatchMilli = StopWatch<std::chrono::milliseconds>;

}  // namespace log

}  // namespace fmr

namespace fmt {

template <typename Duration>
struct formatter<fmr::log::StopWatch<Duration>> : formatter<Duration> {
  template <typename FormatContext>
  auto format(const fmr::log::StopWatch<Duration> &sw, FormatContext &ctx)
      -> decltype(ctx.out()) {
    return formatter<Duration>::format(sw.Elapsed(), ctx);
  }
};

}  // namespace fmt

#endif /* end of include guard: H_FMR_LOG_STOPWATCH_H */
