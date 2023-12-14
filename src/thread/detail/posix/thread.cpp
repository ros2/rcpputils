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

#include <pthread.h>

#if __linux__
#include <sched.h>
#endif

#include <stdexcept>
#include <iostream>

#include "rcpputils/thread/detail/posix/thread.hpp"
#include "rcpputils/thread/detail/posix/utilities.hpp"

namespace rcpputils
{

using thread::detail::ThreadFuncBase, thread::detail::throw_if_error;
using thread::detail::sched_policy_explicit_bit;

namespace
{

void set_pthread_attr(pthread_attr_t & native_attr, ThreadAttribute const & attr);
void * thread_main(void * p);
inline bool is_explicit_sched_policy(int native_policy);
inline int to_native_sched_policy(SchedPolicy policy);

}  // namespace

Thread::Thread(Attribute const * attr, std::unique_ptr<ThreadFuncBase> func)
: handle_(NativeHandleType{})
{
  pthread_attr_t native_attr;
  int r = pthread_attr_init(&native_attr);
  throw_if_error(r, "error in pthread_attr_init");

  if (attr != nullptr) {
    set_pthread_attr(native_attr, *attr);
  }

  NativeHandleType h;
  r = pthread_create(&h, &native_attr, thread_main, func.get());
  throw_if_error(r, "error in pthread_create");

  if (attr == nullptr || !attr->get_run_as_detached()) {
    this->handle_ = h;
  }

  pthread_attr_destroy(&native_attr);

  func.release();
}

void Thread::apply_attr(Attribute const & attr)
{
#if __linux__
  int r;
  SchedPolicy policy = attr.get_sched_policy();
  if (policy == SchedPolicy::inherit) {
    return;
  }
  int native_policy = to_native_sched_policy(policy);
  if (native_policy != SCHED_FIFO && native_policy != SCHED_RR && native_policy != SCHED_OTHER) {
    sched_param param;
    param.sched_priority = attr.get_priority();
    r = pthread_setschedparam(pthread_self(), native_policy, &param);
    throw_if_error(r, "error in pthread_setschedparam");
  }
#endif  // #if __linux__
}

namespace
{

void * thread_main(void * p)
{
  std::unique_ptr<ThreadFuncBase> func(reinterpret_cast<ThreadFuncBase *>(p));

  try {
    func->run();
  } catch (...) {
    std::cerr << "failed to run thread" << std::endl;
    std::terminate();
  }

  return nullptr;
}

void set_pthread_attr(pthread_attr_t & native_attr, ThreadAttribute const & attr)
{
  int r;

#if __linux__
  CpuSet cpu_set = attr.get_affinity();
  if (cpu_set.count()) {
    std::size_t alloc_size = CPU_ALLOC_SIZE(cpu_set.num_processors());
    CpuSet::NativeCpuSetType native_cpu_set = cpu_set.native_cpu_set();
    if (CPU_COUNT(native_cpu_set) > 0) {
      r = pthread_attr_setaffinity_np(&native_attr, alloc_size, native_cpu_set);
    }
    CPU_FREE(native_cpu_set);
    throw_if_error(r, "error in pthread_attr_setaffinity_np");
  }
#endif

  std::size_t stack_size = attr.get_stack_size();
  r = pthread_attr_setstacksize(&native_attr, stack_size);
  throw_if_error(r, "error in pthread_attr_setstacksize");

  int flag = attr.get_run_as_detached() ? PTHREAD_CREATE_DETACHED : PTHREAD_CREATE_JOINABLE;
  r = pthread_attr_setdetachstate(&native_attr, flag);
  throw_if_error(r, "error in pthread_attr_setdetachstate");

  SchedPolicy policy = attr.get_sched_policy();
  if (policy == SchedPolicy::inherit) {
    r = pthread_attr_setinheritsched(&native_attr, PTHREAD_INHERIT_SCHED);
  } else {
    bool has_attr_sched_option = policy == SchedPolicy::other;
#if defined(SCHED_FIFO)
    has_attr_sched_option |= policy == SchedPolicy::fifo;
#endif
#if defined(SCHED_RR)
    has_attr_sched_option |= policy == SchedPolicy::rr;
#endif

    if (has_attr_sched_option) {
      r = pthread_attr_setinheritsched(&native_attr, PTHREAD_EXPLICIT_SCHED);
      throw_if_error(r, "error in pthread_attr_setinheritsched");

      r = pthread_attr_setschedpolicy(&native_attr, to_native_sched_policy(policy));
      throw_if_error(r, "error in pthread_attr_setschedpolicy");

      sched_param param;
      param.sched_priority = attr.get_priority();
      r = pthread_attr_setschedparam(&native_attr, &param);
      throw_if_error(r, "error in pthread_attr_setschedparam");
    }
  }
}

bool is_explicit_sched_policy(int native_policy)
{
  return (static_cast<unsigned>(native_policy) & sched_policy_explicit_bit) != 0;
}

int to_native_sched_policy(rcpputils::SchedPolicy policy)
{
  return static_cast<unsigned>(policy) & ~sched_policy_explicit_bit;
}

}  // namespace

}  // namespace rcpputils
