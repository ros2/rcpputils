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

#ifndef RCPPUTILS__THREAD__DETAIL__POSIX__SCHED_OPTIONS_HPP_
#define RCPPUTILS__THREAD__DETAIL__POSIX__SCHED_OPTIONS_HPP_

#include <optional>
#include <utility>

#include "rcpputils/thread/detail/posix/cpu_set.hpp"
#include "rcpputils/thread/detail/posix/sched_policy.hpp"
#include "rcpputils/visibility_control.hpp"

namespace rcpputils
{

struct SchedOptions
{
  void swap(SchedOptions & other)
  {
    using std::swap;
    swap(policy, other.policy);
    swap(priority, other.priority);
    swap(core_affinity, other.core_affinity);
  }

  std::optional<int> priority;
  std::optional<SchedPolicy> policy;
  std::optional<CpuSet> core_affinity;
};

inline void swap(SchedOptions & a, SchedOptions & b)
{
  a.swap(b);
}

}  // namespace rcpputils

#endif  // RCPPUTILS__THREAD__DETAIL__POSIX__SCHED_OPTIONS_HPP_
