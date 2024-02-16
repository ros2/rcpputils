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

#ifndef RCPPUTILS__THREAD__DETAIL__POSIX__LINUX__CPU_SET_HPP_
#define RCPPUTILS__THREAD__DETAIL__POSIX__LINUX__CPU_SET_HPP_

#include <pthread.h>
#include <memory>
#include <utility>

#include "rcutils/thread_attr.h"
#include "rcpputils/thread/detail/posix/utilities.hpp"

#include "rcpputils/visibility_control.hpp"

namespace rcpputils
{

namespace thread
{
namespace detail
{

struct CpuSetDeleter
{
  void operator()(cpu_set_t * p) const;
};
using UniqueNativeCpuSet = std::unique_ptr<cpu_set_t, CpuSetDeleter>;

}  // namespace detail
}  // namespace thread

struct CpuSet
{
  using NativeCpuSetType = cpu_set_t *;

  CpuSet() = default;
  explicit CpuSet(rcutils_thread_core_affinity_t const & affinity);
  CpuSet(const CpuSet & other);
  CpuSet(CpuSet && other);

  CpuSet & operator=(const CpuSet & other);
  CpuSet & operator=(CpuSet && other);
  void swap(CpuSet & other);
  void set(std::size_t cpu);
  void unset(std::size_t cpu);
  void clear();
  bool is_set(std::size_t cpu) const;
  std::size_t count() const;

  void set_rcutils_thread_core_affinity(rcutils_thread_core_affinity_t const & affinity);

  static std::size_t num_processors();
  CpuSet::NativeCpuSetType native_cpu_set() const;

private:
  void init_cpu_set();
  void valid_cpu(std::size_t cpu) const;
  static std::size_t alloc_size();
  thread::detail::UniqueNativeCpuSet cpu_set_;
};

inline void swap(CpuSet & a, CpuSet & b)
{
  a.swap(b);
}

}  // namespace rcpputils

#endif  // RCPPUTILS__THREAD__DETAIL__POSIX__LINUX__CPU_SET_HPP_
