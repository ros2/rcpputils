// Copyright 2023 Open Source Robotics Foundation, Inc.
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

#ifndef RCPPUTILS__MUTEX_HPP_
#define RCPPUTILS__MUTEX_HPP_

#include <mutex>

#include "rcpputils/visibility_control.hpp"

namespace rcpputils
{
#ifndef RCPPUTILS_USE_PIMUTEX

// Fallback code path
using PIMutex = std::mutex;
using RecursivePIMutex = std::recursive_mutex;

#else

/**
 * Mutex with enabled thread priority inheritance.
 * In case your OS does not support thread priority inheritance for mutexes, or
 * the CMake option USE_PI_MUTEX is turned off, then this class is just an alias for
 * std::mutex.
  **/
class PIMutex : public std::mutex
{
public:
  /// Creates a mutex with priority inheritance enabled
  RCPPUTILS_PUBLIC
  PIMutex();

  /// Releases the mutex
  RCPPUTILS_PUBLIC
  ~PIMutex();
};

/**
 * Recursive mutex with enabled thread priority inheritance.
 * In case your OS does not support thread priority inheritance for mutexes, or
 * the CMake option USE_PI_MUTEX is turned off, then this class is just an alias for
 * std::recursive_mutex.
 **/
class RecursivePIMutex : public std::recursive_mutex
{
public:
  /// Creates a recursive mutex with priority inheritance enabled
  RCPPUTILS_PUBLIC
  RecursivePIMutex();

  /// Releases the mutex
  RCPPUTILS_PUBLIC
  ~RecursivePIMutex();
};

#endif  // RCPPUTILS_USE_PIMUTEX
}  // namespace rcpputils
#endif  // RCPPUTILS__MUTEX_HPP_
