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

#include <sys/types.h>
#include <sched.h>

#include <stdexcept>
#include <iostream>

#include "rcpputils/thread/detail/posix/thread.hpp"
#include "rcpputils/thread/detail/posix/utilities.hpp"
#include "./thread_impl.hpp"

namespace rcpputils
{

namespace this_thread
{

using thread::detail::UniqueNativeCpuSet;
using thread::detail::make_unique_native_cpu_set;
using thread::detail::throw_if_error;
using thread::detail::to_native_sched_policy;

void apply_sched_options(SchedOptions const & options)
{
  pid_t tid = gettid();
  if (options.policy) {
    int native_sched_policy = to_native_sched_policy(*options.policy);
    sched_param param;
    int r;
    if (options.priority) {
      param.sched_priority = *options.priority;
    } else {
      r = sched_getparam(tid, &param);
      throw_if_error(r, "error in sched_getparam");
    }
    r = sched_setscheduler(tid, native_sched_policy, &param);
    throw_if_error(r, "error in sched_setscheduler");
  } else if (options.priority) {
    sched_param param;
    param.sched_priority = *options.priority;
    int r = sched_setparam(tid, &param);
    throw_if_error(r, "error in sched_setparam");
  }
  if (options.core_affinity) {
    UniqueNativeCpuSet native_cpu_set = make_unique_native_cpu_set(*options.core_affinity);
    std::size_t sz = CPU_ALLOC_SIZE(options.core_affinity->count());
    int r = sched_setaffinity(tid, sz, native_cpu_set.get());
    throw_if_error(r, "error in sched_setaffinity");
  }
}

}  // namespace this_thread
}  // namespace rcpputils
