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

#include "rcpputils/thread/detail/posix/linux/cpu_set.hpp"

#include <string.h>
#include <unistd.h>

#include <climits>
#include <utility>

#include "rcutils/thread_attr.h"
#include "rcpputils/thread/detail/posix/utilities.hpp"

static inline std::size_t as_mem_size(std::size_t n)
{
  return (n + CHAR_BIT - 1) / CHAR_BIT;
}

namespace rcpputils
{

using thread::detail::throw_if_error;

CpuSet::CpuSet(rcutils_thread_core_affinity_t const & src_set)
{
  init_cpu_set();
  CPU_ZERO_S(alloc_size(), cpu_set_.get());
  std::size_t dst_core_count = num_processors();
  std::size_t dst_mem_size = alloc_size();
  std::size_t src_mem_size = as_mem_size(src_set.core_count);
  std::size_t copy_size = std::min(src_mem_size, dst_mem_size);
  // this memcpy dependent to structure of cpu_set_t that only have integer array used as bitset.
  memcpy(cpu_set_.get(), src_set.set, copy_size);
  if (src_set.core_count > dst_core_count) {
    for (std::size_t i = dst_core_count; i < src_set.core_count; ++i) {
      if (rcutils_thread_core_affinity_is_set(&src_set, i)) {
        auto ec = std::make_error_code(std::errc::invalid_argument);
        throw std::system_error{ec, "invalid cpu number"};
      }
    }
  } else {
    memset(reinterpret_cast<char *>(cpu_set_.get()) + src_mem_size, 0, dst_mem_size - src_mem_size);
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

std::size_t CpuSet::count() const
{
  if (cpu_set_) {
    return CPU_COUNT_S(num_processors(), cpu_set_.get());
  } else {
    return 0;
  }
}

void CpuSet::set_rcutils_thread_core_affinity(rcutils_thread_core_affinity_t const & affinity)
{
  CpuSet(affinity).swap(*this);
}

std::size_t CpuSet::num_processors()
{
  auto num_proc = sysconf(_SC_NPROCESSORS_ONLN);
  if (num_proc <= 0) {
    throw_if_error(
      num_proc,
      "invalid return value of sysconf(_SC_NPROCESSORS_ONLN)");
  }
  return num_proc;
}

CpuSet::NativeCpuSetType CpuSet::native_cpu_set() const
{
  std::size_t num_proc = num_processors();
  cpu_set_t * result = CPU_ALLOC(num_proc);
  if (!result) {
    throw std::system_error(errno, std::system_category(), "failed to allocate memory");
  }

  std::size_t size = alloc_size();
  if (cpu_set_) {
    memcpy(result, cpu_set_.get(), size);
  } else {
    CPU_ZERO_S(size, result);
  }
  return result;
}

void CpuSet::init_cpu_set()
{
  if (cpu_set_) {
    return;
  }
  auto p = CPU_ALLOC(num_processors());
  CPU_ZERO_S(alloc_size(), p);
  cpu_set_ = std::unique_ptr<cpu_set_t, CpuSetDeleter>(p);
}

void CpuSet::valid_cpu(std::size_t cpu) const
{
  if (num_processors() <= cpu) {
    auto ec = std::make_error_code(std::errc::invalid_argument);
    throw std::system_error{ec, "invalid cpu number"};
  }
}

std::size_t CpuSet::alloc_size()
{
  return CPU_ALLOC_SIZE(num_processors());
}

void CpuSet::CpuSetDeleter::operator()(NativeCpuSetType cpu_set) const
{
  CPU_FREE(cpu_set);
}

}  // namespace rcpputils
