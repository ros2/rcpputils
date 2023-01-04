// Copyright (c) 2020 Robert Bosch GmbH
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

#ifndef RCPPUTILS__THREAD_CONFIG_HPP_
#define RCPPUTILS__THREAD_CONFIG_HPP_

#include <thread>

#ifdef _WIN32  // i.e., Windows platform.
// #include <windows.h>
#elif __APPLE__  // i.e., macOS platform.
// #include <pthread.h>
// #include <mach/mach_init.h>
// #include <mach/mach_port.h>
// #include <mach/mach_time.h>
// #include <mach/thread_act.h>
// #include <mach/thread_policy.h>
// #include <sys/sysctl.h>
#else  // POSIX platforms
  #include <pthread.h>
  #ifdef __QNXNTO__
    #include <sys/neutrino.h>
    #include <sys/syspage.h>
  #endif  // __QNXNTO__
#endif

namespace rcpputils
{

/// Enum for simple configuration of threads in two priority classes.
enum class ThreadPriority
{
  LOW,
  MEDIUM,
  HIGH
};

/// Calculates an OS specific thread priority from a ThreadPriority value.
/**
 * \param[in] thread_priority thread priority to be converted
 * \param[out] os_priority OS specific thread priority
 * \return true on systems that support POSIX
 */
inline bool calculate_os_thread_priority(
  const ThreadPriority thread_priority,
  int & os_priority)
{
#ifdef _WIN32
  return false;
#elif __APPLE__
  return false;
#else
  if (thread_priority == ThreadPriority::HIGH) {
    os_priority = sched_get_priority_max(SCHED_FIFO);
  } else if (thread_priority == ThreadPriority::LOW) {
    os_priority = sched_get_priority_min(SCHED_FIFO);
  } else if (thread_priority == ThreadPriority::MEDIUM) {
    // Should be a value of 49 on standard Linux platforms, which is just below
    // the default priority of 50 for threaded interrupt handling.
    os_priority =
      (sched_get_priority_min(SCHED_FIFO) + sched_get_priority_max(SCHED_FIFO)) / 2 - 1;
  } else {  // unhandled priority
    return false;
  }
  return true;
#endif
}

/// Sets the priority and cpu affinity of the given native thread.
/**
 * This function intentionally only works on operating systems which support a FIFO thread scheduler.
 * Note for Linux: using this function requires elevated privileges and a kernel with realtime patch.
 *
 * Implementation note: For setting thread priorities which are intended for a non-realtime/fair thread
 * scheduler a new utility function should be implemented in order to not mix up different use cases.
 *
 * \param[in] native_handle native thread handle
 * \param[in] priority priority to set for the given thread
 * \param[in] cpu_bitmask cpu core bitmask for the given thread
 * \return true on success
 */
template<typename T>
bool configure_native_realtime_thread(
  T native_handle, const ThreadPriority priority,
  const unsigned int cpu_bitmask = (unsigned) -1)
{
  bool success = true;
#ifdef _WIN32
  return false;
#elif __APPLE__
  return false;
#else  // POSIX systems
  sched_param params;
  int policy;
  success &= (pthread_getschedparam(native_handle, &policy, &params) == 0);
  success &= calculate_os_thread_priority(priority, params.sched_priority);
  success &= (pthread_setschedparam(native_handle, SCHED_FIFO, &params) == 0);

#ifdef __QNXNTO__
  // run_mask is a bit mask to set which cpu a thread runs on
  // where each bit corresponds to a cpu core
  int64_t run_mask = cpu_bitmask;

  // Function used to change thread affinity of thread associated with native_handle
  if (ThreadCtlExt(
      0, native_handle, _NTO_TCTL_RUNMASK,
      reinterpret_cast<void *>(run_mask)) == -1)
  {
    success &= 0;
  } else {
    success &= 1;
  }
#else
  cpu_set_t cpuset;
  CPU_ZERO(&cpuset);
  for (unsigned int i = 0; i < sizeof(cpu_bitmask) * 8; i++) {
    if ( (cpu_bitmask & (1 << i)) != 0) {
      CPU_SET(i, &cpuset);
    }
  }
  success &= (pthread_setaffinity_np(native_handle, sizeof(cpu_set_t), &cpuset) == 0);
#endif  // __QNXNTO__
#endif

  return success;
}

/// Sets the priority and cpu affinity of the given std thread.
/**
 * This function intentionally only works on operating systems which support a FIFO thread scheduler.
 * Note for Linux: using this function requires elevated privileges.
 * Implementation note: For setting thread priorities which are intended for a non-realtime/fair thread scheduler a new utility function should be implemented.
 *
 * \param[in] thread std thread instance
 * \param[in] priority priority to set for the given thread
 * \param[in] cpu_bitmask cpu core bitmask for the given thread
 * \return true on success
 */
inline bool configure_realtime_thread(
  std::thread & thread, const ThreadPriority priority,
  const unsigned int cpu_bitmask = (unsigned) -1)
{
  return configure_native_realtime_thread(thread.native_handle(), priority, cpu_bitmask);
}

}  // namespace rcpputils

#endif  // RCPPUTILS__THREAD_CONFIG_HPP_
