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

#ifndef RCPPUTILS__UNIQUE_LOCK_HPP_
#define RCPPUTILS__UNIQUE_LOCK_HPP_

#include <mutex>

#include "rcpputils/thread_safety_annotations.hpp"

namespace rcpputils
{

/**
 * @brief Trivial std::unique_lock wrapper providing constructor that allows Clang's
 * Thread Safety Analysis.
 * The libc++ std::unique_lock does not have these annotations.
 */
template<typename MutexT>
class RCPPUTILS_TSA_SCOPED_CAPABILITY unique_lock : public std::unique_lock<MutexT>
{
public:
  explicit unique_lock(MutexT & mu) RCPPUTILS_TSA_ACQUIRE(mu)
  : std::unique_lock<MutexT>(mu)
  {}

  ~unique_lock() RCPPUTILS_TSA_RELEASE() {}
};

}  // namespace rcpputils

#endif  // RCPPUTILS__UNIQUE_LOCK_HPP_
