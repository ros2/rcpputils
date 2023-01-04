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

#include "rcutils/thread.h"
#include "rcpputils/visibility_control.hpp"

namespace rcpputils
{

/// Sets the priority and cpu affinity of the given std thread.
/**
 * This function intentionally only works on operating systems which support a FIFO thread scheduler.
 * Note for Linux: using this function requires elevated privileges.
 * Implementation note: For setting thread priorities which are intended for a non-realtime/fair
 *                      thread scheduler a new utility function should be implemented.
 *
 * \param[in] thread std thread instance
 * \param[in] priority priority to set for the given thread
 * \param[in] cpu_bitmask cpu core bitmask for the given thread
 * \return true on success
 */
RCPPUTILS_PUBLIC
bool configure_realtime_thread(
  std::thread & thread, const ThreadPriority priority,
  const unsigned int cpu_bitmask = (unsigned) -1)
{
  rcutils_ret_t return_value = configure_native_realtime_thread(thread.native_handle(),
                                                                priority, cpu_bitmask);
  return return_value == RCUTILS_RET_OK ? true : false;
}

}  // namespace rcpputils

#endif  // RCPPUTILS__THREAD_CONFIG_HPP_
