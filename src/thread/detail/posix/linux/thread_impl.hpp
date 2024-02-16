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

#ifndef THREAD__DETAIL__POSIX__LINUX__THREAD_IMPL_HPP_
#define THREAD__DETAIL__POSIX__LINUX__THREAD_IMPL_HPP_

#include <memory>

#include "rcpputils/thread/detail/posix/linux/cpu_set.hpp"

namespace rcpputils
{
namespace thread
{
namespace detail
{

inline UniqueNativeCpuSet make_unique_native_cpu_set(CpuSet const & cpu_set)
{
  return UniqueNativeCpuSet{cpu_set.native_cpu_set()};
}

}  // namespace detail
}  // namespace thread
}  // namespace rcpputils

#endif  // THREAD__DETAIL__POSIX__LINUX__THREAD_IMPL_HPP_
