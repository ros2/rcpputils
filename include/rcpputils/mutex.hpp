// Copyright 2020 Open Source Robotics Foundation, Inc.
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

/**
 * Mutex with priority inheritance on systems that support it.
 * This class derives from std::mutex to be fully compatible with standard C++.
 * This implementation is a workaround that is needed until the C++ standard library offers the same mutex functionality.
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
 * Recursive mutex with priority inheritance on systems that support it.
 * This class derives from std::recursive_mutex to be fully compatible with standard C++.
 * This implementation is a workaround that is needed until the C++ standard library offers the same mutex functionality.
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

}  // namespace rcpputils

#endif  // RCPPUTILS__MUTEX_HPP_
