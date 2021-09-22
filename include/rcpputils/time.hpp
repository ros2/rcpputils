// Copyright 2021 Open Source Robotics Foundation, Inc.
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

#ifndef RCPPUTILS__TIME_HPP_
#define RCPPUTILS__TIME_HPP_

#include <chrono>

#include "rcutils/time.h"

namespace rcpputils
{

/// Convert to std::chrono::nanoseconds.
/**
 * This function help to convert from std::chrono::duration to std::chrono::nanoseconds and throw
 * exception if overflow occurs while coverting.
 *
 * \param[in] time The time to be converted to std::chrono::nanoseconds.
 * \return std::chrono::nanoseconds.
 * \throws std::invalid_argument if time is bigger than std::chrono::nanoseconds::max().
 */
template<typename DurationRepT = int64_t, typename DurationT = std::milli>
std::chrono::nanoseconds convert_to_nanoseconds(
  const std::chrono::duration<DurationRepT, DurationT> & time)
{
  // Casting to a double representation might lose precision and allow the check below to succeed
  // but the actual cast to nanoseconds fail. Using 1 DurationT worth of nanoseconds less than max
  constexpr auto maximum_safe_cast_ns =
    std::chrono::nanoseconds::max() - std::chrono::duration<DurationRepT, DurationT>(1);
  // If period is greater than nanoseconds::max(), the duration_cast to nanoseconds will overflow
  // a signed integer, which is undefined behavior. Checking whether any std::chrono::duration is
  // greater than nanoseconds::max() is a difficult general problem. This is a more conservative
  // version of Howard Hinnant's (the <chrono> guy>) response here:
  // https://stackoverflow.com/a/44637334/2089061
  // However, this doesn't solve the issue for all possible duration types of period.
  // Follow-up issue: https://github.com/ros2/rclcpp/issues/1177
  constexpr auto ns_max_as_double =
    std::chrono::duration_cast<std::chrono::duration<double, std::chrono::nanoseconds::period>>(
    maximum_safe_cast_ns);
  if (time > ns_max_as_double) {
    throw std::invalid_argument{
            "time must be less than std::chrono::nanoseconds::max()"};
  }

  return std::chrono::duration_cast<std::chrono::nanoseconds>(time);
}

}  // namespace rcpputils

#endif  // RCPPUTILS__TIME_HPP_
