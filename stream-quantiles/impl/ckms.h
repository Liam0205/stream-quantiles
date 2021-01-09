// =========================================================================
// Copyright 2021 -- present Liam Huang (Yuuki) [liamhuang0205@gmail.com].
// Author: Liam Huang
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// =========================================================================

#pragma once

#include <cassert>
#include <cmath>
#include <list>

namespace yuuki {
namespace ckms {
template <typename T>
struct triple {
  T v;
  int g;
  int delta;

  triple(T v, int g, int delta) : v(v), g(g), delta(delta) {
  }
};

template <typename T>
class implement {
 protected:
  using triple_t = triple<T>;
  // TODO(Liam): Follow the paper, this might be replaced with `multiset`.
  using triples_t = std::list<triple_t>;
  using iterator_t = typename triples_t::iterator;
  using const_iterator_t = typename triples_t::const_iterator;
  triples_t S_;
  int n_;

 public:
  implement() : S_(), n_(0) {
  }

  virtual ~implement() = default;

  implement(const implement&) = default;
  implement& operator=(const implement&) = default;
  implement(implement&&) = default;
  implement& operator=(implement&&) = default;

 public:
  void insert(T v) {
    do_insert(v);
    if (compress_condition()) compress();
  }

  T quantile(double phi) const {
    if (S_.empty()) return T{};

    double phin = phi * n_;
    // TODO(Liam): f(phin, n_) might be a bug.
    double comp = phin + f(phin, n_) / 2;

    int r_i = 0;
    const_iterator_t it = S_.cbegin();
    const_iterator_t end = S_.cend();
    assert(it != end);

    const_iterator_t it_prev = it;
    ++it;
    for (; it != end; ++it, ++it_prev) {
      r_i += it_prev->g;
      if (r_i + it->g + it->delta > comp) return it_prev->v;
    }
    return it_prev->v;
  }

 protected:
  virtual bool compress_condition() const = 0;
  virtual double f(double r_i, int n) const = 0;

 private:
  void do_insert(T v) {
    int r_i = 0;
    iterator_t it = S_.begin();
    while (it != S_.end() && it->v < v) {
      r_i += it->g;
      ++it;
    }
    int delta = static_cast<int>(std::floor(f(r_i, n_))) - 1;
    delta = std::max(0, delta);
    triple_t t(v, 1, delta);
    S_.insert(it, t);
    ++n_;
  }

  void compress() {
    if (S_.empty()) return;

    int r_i = 0;
    iterator_t it;
    for (it = S_.begin(); it != S_.end(); ++it) {
      r_i += it->g;
    }
    --it;
    if (it == S_.begin()) return;
    --it;

    for (; it != S_.begin(); --it) {
      iterator_t it2 = next(it);
      r_i -= it->g;
      if (it->g + it2->g + it2->delta <= f(r_i, n_)) {
        merge(it, it2);
      }
    }
  }

  void merge(iterator_t it1, iterator_t it2) {
    it1->v = it2->v;
    it1->g = it1->g + it2->g;
    it1->delta = it2->delta;
    S_.erase(it2);
  }
};
}  // namespace ckms
}  // namespace yuuki
