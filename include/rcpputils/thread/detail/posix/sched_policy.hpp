// Copyright 2024 eSOL Co.,Ltd.
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

#ifndef RCPPUTILS__THREAD__DETAIL__POSIX__SCHED_POLICY_HPP_
#define RCPPUTILS__THREAD__DETAIL__POSIX__SCHED_POLICY_HPP_

#include <sched.h>

#include "rcutils/thread_attr.h"

#include "rcpputils/visibility_control.hpp"

namespace rcpputils
{

namespace thread
{
namespace detail
{

constexpr unsigned int sched_policy_explicit_bit = 0x8000'0000;

}  // namespace detail
}  // namespace thread

enum struct SchedPolicy : unsigned int
{
  inherit,
  other = thread::detail::sched_policy_explicit_bit | SCHED_OTHER,
#ifdef SCHED_FIFO
  fifo = thread::detail::sched_policy_explicit_bit | SCHED_FIFO,
#endif
#ifdef SCHED_RR
  rr = thread::detail::sched_policy_explicit_bit | SCHED_RR,
#endif
#ifdef SCHED_IDLE
  idle = thread::detail::sched_policy_explicit_bit | SCHED_IDLE,
#endif
#ifdef SCHED_BATCH
  batch = thread::detail::sched_policy_explicit_bit | SCHED_BATCH,
#endif
#ifdef SCHED_SPORADIC
  sporadic = thread::detail::sched_policy_explicit_bit | SCHED_SPORADIC,
#endif
// #if __linux__
// linux deadline scheduler requires more parameter, not supported now
// #ifdef SCHED_DEADLINE
//   deadline = SCHED_DEADLINE,
// #endif
// #endif
};

SchedPolicy from_rcutils_thread_scheduling_policy(
  rcutils_thread_scheduling_policy_t rcutils_sched_policy);

}  // namespace rcpputils

#endif  // RCPPUTILS__THREAD__DETAIL__POSIX__SCHED_POLICY_HPP_
