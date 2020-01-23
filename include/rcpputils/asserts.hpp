// Copyright 2020 Amazon.com, Inc. or its affiliates. All Rights Reserved.
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

#ifndef RCPPUTILS__ASSERTS_HPP_
#define RCPPUTILS__ASSERTS_HPP_

#include <stdexcept>

namespace rcpputils
{
/**
 * Checks that a state condition passes.
 *
 * \param condition
 * \throw std::runtime_error if the condition is not met.
 */
inline void check(bool condition)
{
  if (!condition) {
    throw std::runtime_error{"Check reported invalid state!"};
  }
}

/**
 * Asserts that a condition passes.
 *
 * This function behaves similar to assert, except that it throws instead of invoking abort().
 * \param condition
 * \throw std::runtime_error if the macro NDEBUG is not set and the condition is not met.
 */
inline void ros_assert(bool condition)
{
// Same macro definition used by cassert
#ifndef NDEBUG
  if (!condition) {
    throw std::runtime_error{"Assertion failed!"};
  }
#else
  (void) condition;
#endif
}
}  // namespace rcpputils

#endif  // RCPPUTILS__ASSERTS_HPP_
