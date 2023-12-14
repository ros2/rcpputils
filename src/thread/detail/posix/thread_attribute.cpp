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

#include "rcpputils/thread/detail/posix/thread_attribute.hpp"

#include <pthread.h>

#include "rcpputils/scope_exit.hpp"
#include "rcpputils/thread/detail/posix/utilities.hpp"

namespace rcpputils
{

static inline SchedPolicy from_native_sched_policy(int native_policy);

using thread::detail::throw_if_error;
using thread::detail::sched_policy_explicit_bit;

ThreadAttribute::ThreadAttribute()
{
  pthread_attr_t pt_attr;
  int r;
  int native_policy;

  r = pthread_attr_init(&pt_attr);
  throw_if_error(r, "error in pthread_attr_init");

  RCPPUTILS_SCOPE_EXIT(pthread_attr_destroy(&pt_attr));

  r = pthread_attr_getschedpolicy(&pt_attr, &native_policy);
  throw_if_error(r, "error in pthread_attr_getschedpolicy");

  int explicit_sched;
  r = pthread_attr_getinheritsched(&pt_attr, &explicit_sched);
  throw_if_error(r, "error in pthread_attr_getinheritedsched");
  if (explicit_sched == PTHREAD_EXPLICIT_SCHED) {
    sched_policy_ = from_native_sched_policy(native_policy);
  } else {
    sched_policy_ = SchedPolicy::inherit;
  }

  r = pthread_attr_getstacksize(&pt_attr, &stack_size_);
  throw_if_error(r, "error in pthread_attr_getstacksize");

  sched_param param;
  r = pthread_attr_getschedparam(&pt_attr, &param);
  throw_if_error(r, "error in pthread_attr_getschedparam");
  priority_ = param.sched_priority;

  int flag;
  r = pthread_attr_getdetachstate(&pt_attr, &flag);
  throw_if_error(r, "error in pthread_attr_getdetachstate");
  detached_flag_ = (flag == PTHREAD_CREATE_DETACHED);
}

ThreadAttribute::ThreadAttribute(const rcutils_thread_attr_t & attr)
: cpu_set_(CpuSet(attr.core_affinity)),
  sched_policy_(from_rcutils_thread_scheduling_policy(attr.scheduling_policy)),
  priority_(attr.priority)
{
  pthread_attr_t pt_attr;
  int r;

  r = pthread_attr_init(&pt_attr);
  throw_if_error(r, "error in pthread_attr_init");

  RCPPUTILS_SCOPE_EXIT(pthread_attr_destroy(&pt_attr));

  r = pthread_attr_getstacksize(&pt_attr, &stack_size_);
  throw_if_error(r, "error in pthread_attr_getstacksize");

  int flag;
  r = pthread_attr_getdetachstate(&pt_attr, &flag);
  throw_if_error(r, "error in pthread_attr_getdetachstate");
  detached_flag_ = (flag == PTHREAD_CREATE_DETACHED);
}

SchedPolicy from_rcutils_thread_scheduling_policy(
  rcutils_thread_scheduling_policy_t rcutils_sched_policy)
{
  switch (rcutils_sched_policy) {
    case RCUTILS_THREAD_SCHEDULING_POLICY_OTHER:
      return SchedPolicy::other;
#ifdef SCHED_FIFO
    case RCUTILS_THREAD_SCHEDULING_POLICY_FIFO:
      return SchedPolicy::fifo;
#endif
#ifdef SCHED_RR
    case RCUTILS_THREAD_SCHEDULING_POLICY_RR:
      return SchedPolicy::rr;
#endif
#ifdef SCHED_IDLE
    case RCUTILS_THREAD_SCHEDULING_POLICY_IDLE:
      return SchedPolicy::idle;
#endif
#ifdef SCHED_BATCH
    case RCUTILS_THREAD_SCHEDULING_POLICY_BATCH:
      return SchedPolicy::batch;
#endif
#ifdef SCHED_SPORADIC
    case RCUTILS_THREAD_SCHEDULING_POLICY_SPORADIC:
      return SchedPolicy::sporadic;
#endif
// #ifdef SCHED_DEADLINE
//     case RCUTILS_THREAD_SCHEDULING_POLICY_DEADLINE:
//       return SCHED_DEADLINE;
// #endif
    default:
      throw std::invalid_argument("Invalid scheduling policy");
  }
}

SchedPolicy from_native_sched_policy(int native_policy)
{
  return SchedPolicy(native_policy | sched_policy_explicit_bit);
}

}  // namespace rcpputils
