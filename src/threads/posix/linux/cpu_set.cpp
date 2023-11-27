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

#include "rcpputils/threads/posix/linux/cpu_set.hpp"

#include <string.h>
#include <unistd.h>
#include <utility>

#include "rcutils/thread_attr.h"

namespace rcpputils
{

namespace detail
{

CpuSet::CpuSet(rcutils_thread_core_affinity_t const & affinity)
{
  init_cpu_set();
  CPU_ZERO_S(alloc_size(), cpu_set_.get());
  std::size_t size = std::min(CPU_ALLOC_SIZE(affinity.core_count), alloc_size());
  // this memcpy dependent to structure of cpu_set_t that only have integer array used as bitset.
  memcpy(cpu_set_.get(), affinity.set, size);
  if (affinity.core_count > num_proc_) {
    for (std::size_t i = num_proc_; i < affinity.core_count; ++i) {
      if (rcutils_thread_core_affinity_is_set(&affinity, i)) {
        auto ec = std::make_error_code(std::errc::invalid_argument);
        throw std::system_error{ec, "invalid cpu number"};
      }
    }
  }
}

CpuSet::CpuSet(const CpuSet & other)
{
  if (!other.cpu_set_) {
    return;
  }
  init_cpu_set();
  memcpy(cpu_set_.get(), other.cpu_set_.get(), alloc_size());
}

CpuSet::CpuSet(CpuSet && other)
: CpuSet()
{
  swap(other);
}

CpuSet & CpuSet::operator=(const CpuSet & other)
{
  if (other.cpu_set_) {
    init_cpu_set();
    memcpy(cpu_set_.get(), other.cpu_set_.get(), alloc_size());
  } else {
    clear();
  }
  return *this;
}

CpuSet & CpuSet::operator=(CpuSet && other)
{
  CpuSet tmp;
  other.swap(tmp);
  tmp.swap(*this);
  return *this;
}

void CpuSet::swap(CpuSet & other)
{
  using std::swap;
  swap(cpu_set_, other.cpu_set_);
  swap(num_proc_, other.num_proc_);
}

void CpuSet::set(std::size_t cpu)
{
  init_cpu_set();
  valid_cpu(cpu);
  CPU_SET_S(cpu, alloc_size(), cpu_set_.get());
}

void CpuSet::unset(std::size_t cpu)
{
  init_cpu_set();
  valid_cpu(cpu);
  CPU_CLR_S(cpu, alloc_size(), cpu_set_.get());
}

void CpuSet::clear()
{
  if (cpu_set_) {
    CPU_ZERO_S(alloc_size(), cpu_set_.get());
  }
}

bool CpuSet::is_set(std::size_t cpu) const
{
  if (cpu_set_) {
    valid_cpu(cpu);
    return CPU_ISSET_S(cpu, alloc_size(), cpu_set_.get());
  } else {
    return false;
  }
}

void CpuSet::set_rcutils_thread_core_affinity(rcutils_thread_core_affinity_t const & affinity)
{
  CpuSet(affinity).swap(*this);
}

void CpuSet::init_cpu_set()
{
  if (cpu_set_) {
    return;
  }
  auto num_proc = sysconf(_SC_NPROCESSORS_ONLN);
  if (num_proc <= 0) {
    throw_if_error(
      num_proc,
      "invalid return value of sysconf(_SC_NPROCESSORS_ONLN)");
  }
  auto p = CPU_ALLOC(CPU_ALLOC_SIZE(num_proc));
  cpu_set_ = std::unique_ptr<NativeCpuSetType, CpuSetDeleter>(p);
  num_proc_ = num_proc;
}

void CpuSet::valid_cpu(std::size_t cpu) const
{
  if (num_proc_ <= cpu) {
    auto ec = std::make_error_code(std::errc::invalid_argument);
    throw std::system_error{ec, "invalid cpu number"};
  }
}

void CpuSet::CpuSetDeleter::operator()(NativeCpuSetType * cpu_set) const
{
  CPU_FREE(cpu_set);
}

}  // namespace detail

}  // namespace rcpputils
