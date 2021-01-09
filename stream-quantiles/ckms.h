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

#include <algorithm>
#include <cassert>
#include <limits>

#include "stream-quantiles/impl/ckms.h"

namespace yuuki {
namespace ckms {
template <typename T>
class high_biased final : public implement<T> {
 public:
  explicit high_biased(double epsilon)
      : epsilon_(epsilon), one_over_2e_(static_cast<int>(1 / (2 * epsilon))) {
  }

 protected:
  bool compress_condition() const final {
    assert(one_over_2e_ > 0);
    return (this->n_ % one_over_2e_) == 0;
  }

  double f(double r_i, int n) const final {
    return 2 * epsilon_ * (n - r_i);
  }

 private:
  double epsilon_;
  int one_over_2e_;
};

template <typename T>
class low_biased final : public implement<T> {
 public:
  explicit low_biased(double epsilon) : epsilon_(epsilon), one_over_2e_((int)(1 / (2 * epsilon))) {
  }

 protected:
  bool compress_condition() const final {
    assert(one_over_2e_ > 0);
    return (this->n_ % one_over_2e_) == 0;
  }

  double f(double r_i, int /* n */) const final {
    return 2 * epsilon_ * r_i;
  }

 private:
  double epsilon_;
  int one_over_2e_;
};

template <typename T>
class uniform final : public implement<T> {
 public:
  explicit uniform(double epsilon) : epsilon_(epsilon), one_over_2e_(static_cast<int>(1 / (2 * epsilon))) {
  }

 protected:
  bool compress_condition() const final {
    assert(one_over_2e_ > 0);
    return (this->n_ % one_over_2e_) == 0;
  }

  double f(double /* r_i */, int n) const final {
    return 2 * epsilon_ * n;
  }

 private:
  double epsilon_;
  int one_over_2e_;
};
}  // namespace ckms
}  // namespace yuuki
