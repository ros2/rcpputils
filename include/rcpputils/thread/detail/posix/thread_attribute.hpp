// Copyright 2023 eSOL Co.,Ltd.
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

#ifndef RCPPUTILS__THREAD__DETAIL__POSIX__THREAD_ATTRIBUTE_HPP_
#define RCPPUTILS__THREAD__DETAIL__POSIX__THREAD_ATTRIBUTE_HPP_

#include <pthread.h>

#include <utility>

#include "rcutils/thread_attr.h"

#include "rcpputils/thread/detail/posix/cpu_set.hpp"
#include "rcpputils/visibility_control.hpp"

namespace rcpputils
{

namespace thread
{
namespace detail
{

constexpr unsigned int sched_policy_explicit_bit = 0x8000'0000;

}
}

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

struct ThreadAttribute
{
  ThreadAttribute();

  ThreadAttribute(const ThreadAttribute &) = default;
  ThreadAttribute(ThreadAttribute &&) = default;

  explicit ThreadAttribute(const rcutils_thread_attr_t & attr);

  ThreadAttribute & operator=(const ThreadAttribute &) = default;
  ThreadAttribute & operator=(ThreadAttribute &&) = default;

  ThreadAttribute & set_affinity(CpuSet cs)
  {
    cpu_set_ = std::move(cs);
    return *this;
  }
  const CpuSet & get_affinity() const
  {
    return cpu_set_;
  }

  ThreadAttribute & set_sched_policy(SchedPolicy policy)
  {
    sched_policy_ = policy;
    return *this;
  }
  SchedPolicy get_sched_policy() const
  {
    return sched_policy_;
  }

  ThreadAttribute & set_stack_size(std::size_t sz)
  {
    stack_size_ = sz;
    return *this;
  }
  std::size_t get_stack_size() const
  {
    return stack_size_;
  }

  ThreadAttribute & set_priority(int prio)
  {
    priority_ = prio;
    return *this;
  }
  int get_priority() const
  {
    return priority_;
  }

  ThreadAttribute & set_run_as_detached(bool detach)
  {
    detached_flag_ = detach;
    return *this;
  }
  bool get_run_as_detached() const
  {
    return detached_flag_;
  }

  void
  set_rcutils_thread_attribute(
    const rcutils_thread_attr_t & attr)
  {
    CpuSet cpu_set(attr.core_affinity);
    set_affinity(std::move(cpu_set));
    set_sched_policy(from_rcutils_thread_scheduling_policy(attr.scheduling_policy));
    set_priority(attr.priority);
  }

  void
  swap(
    ThreadAttribute & other)
  {
    using std::swap;
    swap(cpu_set_, other.cpu_set_);
    swap(sched_policy_, other.sched_policy_);
    swap(stack_size_, other.stack_size_);
    swap(priority_, other.priority_);
    swap(detached_flag_, other.detached_flag_);
  }

private:
  CpuSet cpu_set_;
  SchedPolicy sched_policy_;
  std::size_t stack_size_;
  int priority_;
  bool detached_flag_;
};

inline void swap(ThreadAttribute & a, ThreadAttribute & b)
{
  a.swap(b);
}

}  // namespace rcpputils

#endif  // RCPPUTILS__THREAD__DETAIL__POSIX__THREAD_ATTRIBUTE_HPP_
