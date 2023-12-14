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

#ifndef RCPPUTILS__THREAD__DETAIL__STD__CPU_SET_HPP_
#define RCPPUTILS__THREAD__DETAIL__STD__CPU_SET_HPP_

#include <cstddef>
#include <utility>

#include "rcutils/thread_attr.h"
#include "rcpputils/visibility_control.hpp"

namespace rcpputils
{

namespace detail
{

struct EmptyType {};

struct CpuSet
{
  using NativeCpuSetType = EmptyType;
  CpuSet() {}
  explicit CpuSet(const rcutils_thread_core_affinity_t &) {}
  CpuSet(const CpuSet &) {}
  CpuSet & operator=(const CpuSet &)
  {
    return *this;
  }
  CpuSet(CpuSet &&) = delete;
  CpuSet & operator=(CpuSet &&) = delete;
  ~CpuSet() {}
  void swap(CpuSet &) {}
  void set(std::size_t) {}
  void unset(std::size_t) {}
  void clear() {}
  bool is_set(std::size_t)
  {
    return false;
  }
  std::size_t count()
  {
    return 0;
  }
  void set_rcutils_thread_core_affinity(rcutils_thread_core_affinity_t const &) {}
  static std::size_t num_processors()
  {
    return std::thread::hardware_concurrency();
  }
  NativeCpuSetType native_cpu_set() const
  {
    return EmptyType{};
  }
};

inline void swap(CpuSet & a, CpuSet & b)
{
  a.swap(b);
}

}  // namespace detail

using detail::CpuSet;
using detail::swap;

}  // namespace rcpputils

#endif  // RCPPUTILS__THREAD__DETAIL__STD__CPU_SET_HPP_
