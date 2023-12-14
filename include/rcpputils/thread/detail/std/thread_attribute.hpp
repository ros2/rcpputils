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

#ifndef RCPPUTILS__THREAD__DETAIL__STD__THREAD_ATTRIBUTE_HPP_
#define RCPPUTILS__THREAD__DETAIL__STD__THREAD_ATTRIBUTE_HPP_

#include <utility>

#include "rcutils/thread_attr.h"

#include "rcpputils/thread/detail/std/cpu_set.hpp"
#include "rcpputils/visibility_control.hpp"

namespace rcpputils
{

struct Thread;

enum struct SchedPolicy : unsigned
{
};

SchedPolicy from_rcutils_thread_scheduling_policy(
  rcutils_thread_scheduling_policy_t)
{
  return SchedPolicy{};
}

struct ThreadAttribute
{
  ThreadAttribute()
  : set_unavailable_items_(false), run_as_detached_(false) {}

  ThreadAttribute(const ThreadAttribute &) = default;
  ThreadAttribute(ThreadAttribute &&) = default;
  ThreadAttribute & operator=(const ThreadAttribute &) = default;
  ThreadAttribute & operator=(ThreadAttribute &&) = default;

  ThreadAttribute & set_affinity(CpuSet &)
  {
    set_unavailable_items_ = true;
    return *this;
  }
  CpuSet get_affinity()
  {
    return CpuSet{};
  }

  ThreadAttribute & set_stack_size(std::size_t)
  {
    set_unavailable_items_ = true;
    return *this;
  }
  std::size_t get_stack_size() const
  {
    return 0;
  }

  ThreadAttribute & set_priority(int prio)
  {
    (void)prio;
    set_unavailable_items_ = true;
    return *this;
  }
  int get_priority() const
  {
    return 0;
  }

  ThreadAttribute & set_run_as_detached(bool detach)
  {
    run_as_detached_ = detach;
    return *this;
  }
  bool get_run_as_detached() const
  {
    return run_as_detached_;
  }

  void
  set_rcutils_thread_attribute(
    const rcutils_thread_attr_t &)
  {
    set_unavailable_items_ = true;
  }

  void swap(
    ThreadAttribute & other)
  {
    std::swap(*this, other);
  }

private:
  friend struct Thread;
  bool set_unavailable_items_;
  bool run_as_detached_;
};

inline void swap(ThreadAttribute & a, ThreadAttribute & b)
{
  a.swap(b);
}

}  // namespace rcpputils

#endif  // RCPPUTILS__THREAD__DETAIL__STD__THREAD_ATTRIBUTE_HPP_
