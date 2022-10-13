// Copyright 2020 PAL Robotics S.L.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef RCPPMATH__ROLLING_MEAN_ACCUMULATOR_HPP_
#define RCPPMATH__ROLLING_MEAN_ACCUMULATOR_HPP_

#include "rcpputils/rolling_mean_accumulator.hpp"

#warning \
  "the rcppmath namespace is deprecated, include rcpputils/rolling_mean_accumulator.hpp instead"

namespace rcppmath
{

template<typename T>
using RollingMeanAccumulator [[deprecated("use rcpputils::RollingMeanAccumulator instead")]] =
  rcpputils::RollingMeanAccumulator<T>;

}  // namespace rcppmath

#endif  // RCPPMATH__ROLLING_MEAN_ACCUMULATOR_HPP_
