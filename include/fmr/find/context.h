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

#ifndef INCLUDE_FMR_FIND_CONTEXT_H_
#define INCLUDE_FMR_FIND_CONTEXT_H_

namespace fmr {

namespace find {

template <typename T>
class Context {
 public:
  virtual bool Check(const T &check) = 0;
  virtual bool Compare(const T &t1, const T &t2) = 0;
  virtual void Consume(T &t) = 0;
};

template <typename T, typename Checker, typename Comparer, typename Consumer>
class FunctorContext : public Context<T> {
  Checker checker_;
  Comparer comparer_;
  Consumer consumer_;

 public:
  FunctorContext(Checker checker, Comparer comparer, Consumer consumer)
      : checker_(checker), comparer_(comparer), consumer_(consumer) {}
  FunctorContext(FunctorContext &) = default;
  FunctorContext(FunctorContext &&) = default;

  bool Check(const T &check) override { return checker_(check); }
  bool Compare(const T &t1, const T &t2) override { return comparer_(t1, t2); }
  void Consume(T &t) override { return consumer_(t); }
};

template <typename T, typename Checker, typename Comparer, typename Consumer>
FunctorContext<T, Checker, Comparer, Consumer> MakeFunctorContext(
    Checker checker, Comparer comparer, Consumer consumer) {
  return FunctorContext<T, Checker, Comparer, Consumer>(checker, comparer,
                                                        consumer);
}

}  // namespace find

}  // namespace fmr

#endif  // INCLUDE_FMR_FIND_CONTEXT_H_
