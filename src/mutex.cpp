// Copyright 2020 Open Source Robotics Foundation, Inc.
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

#include "rcpputils/mutex.hpp"

#ifndef _WIN32
#include <pthread.h>
#endif

namespace rcpputils
{

#ifndef _WIN32
PIMutex::PIMutex()
{
  // Destroy the underlying mutex
  pthread_mutex_destroy(native_handle());

  // Create mutex attribute with desired settings
  pthread_mutexattr_t attr;
  pthread_mutexattr_init(&attr);
  pthread_mutexattr_setprotocol(&attr, PTHREAD_PRIO_INHERIT);

  // Initialize the underlying mutex
  if (pthread_mutex_init(native_handle(), &attr) != 0) {
    throw std::runtime_error("Mutex initialization failed.");
  }

  // The attribute object isn't needed any more
  pthread_mutexattr_destroy(&attr);
}

RecursivePIMutex::RecursivePIMutex()
{
  // Destroy the underlying mutex
  pthread_mutex_destroy(native_handle());

  // Create mutex attribute with desired settings
  pthread_mutexattr_t attr;
  pthread_mutexattr_init(&attr);
  pthread_mutexattr_setprotocol(&attr, PTHREAD_PRIO_INHERIT);
  pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);

  // Initialize the underlying mutex
  if (pthread_mutex_init(native_handle(), &attr) != 0) {
    throw std::runtime_error("Recursive mutex initialization failed.");
  }

  // The attribute object isn't needed any more
  pthread_mutexattr_destroy(&attr);
}
#else
PIMutex::PIMutex()
{
}

RecursivePIMutex::RecursivePIMutex()
{
}
#endif  // _WIN32

PIMutex::~PIMutex()
{
}

RecursivePIMutex::~RecursivePIMutex()
{
}
}  // namespace rcpputils
