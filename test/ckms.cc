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

#include "stream-quantiles/ckms.h"

#include <catch.h>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <functional>
#include <random>

static const double kEpsilon = 0.01;
static const double kMax = 500;
static const double kPercentile_high_biased[] = {0.5, 0.7, 0.9, 0.95, 0.99};
static const double kPercentile_low_biased[] = {0.01, 0.05, 0.1, 0.3, 0.5};
static const double kPercentile_uniform[] = {0.01, 0.05, 0.1, 0.3, 0.5, 0.7, 0.9, 0.95, 0.99};

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<long double> dis(1, kMax);

namespace yuuki {
namespace ckms {
#define YUUKI_TEST_IT(CKMS, REAL)                                               \
  SECTION(#REAL) {                                                              \
    CKMS<REAL> ckms(kEpsilon);                                                  \
    const int sz = std::min(500 * static_cast<int>(kMax), 1000000);             \
    std::vector<REAL> vec;                                                      \
    vec.reserve(sz);                                                            \
    for (int i = 0; i < sz; ++i) {                                              \
      auto r = dis(gen);                                                        \
      ckms.insert(r);                                                           \
      vec.push_back(r);                                                         \
    }                                                                           \
    std::sort(vec.begin(), vec.end(), std::less<REAL>());                       \
    for (size_t i = 0; i != sizeof(kPercentile_##CKMS) / sizeof(double); ++i) { \
      const double perc = kPercentile_##CKMS[i];                                \
      const double quan = ckms.quantile(perc);                                  \
      const size_t inf = std::floor((1 - kEpsilon) * perc * sz);                \
      const size_t sup = std::ceil((1 + kEpsilon) * perc * sz);                 \
      REQUIRE((vec[inf] <= quan && quan <= vec[sup]));                          \
    }                                                                           \
  }

TEST_CASE("high_biased") {
  YUUKI_TEST_IT(high_biased, float);
  YUUKI_TEST_IT(high_biased, double);
  YUUKI_TEST_IT(high_biased, long double);
}

TEST_CASE("low_biased") {
  YUUKI_TEST_IT(low_biased, float);
  YUUKI_TEST_IT(low_biased, double);
  YUUKI_TEST_IT(low_biased, long double);
}

TEST_CASE("uniform") {
  YUUKI_TEST_IT(uniform, float);
  YUUKI_TEST_IT(uniform, double);
  YUUKI_TEST_IT(uniform, long double);
}
#undef YUUKI_TEST_IT
}  // namespace ckms
}  // namespace yuuki
