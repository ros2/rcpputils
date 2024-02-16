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

#ifndef RCPPUTILS__THREAD__DETAIL__POSIX__UTILITIES_HPP_
#define RCPPUTILS__THREAD__DETAIL__POSIX__UTILITIES_HPP_

#include <memory>
#include <system_error>

#include "rcpputils/thread/detail/posix/sched_policy.hpp"

namespace rcpputils
{
namespace thread
{
namespace detail
{

inline void throw_if_error(int r, char const * msg)
{
  if (r != 0) {
    throw std::system_error(r, std::system_category(), msg);
  }
}

using thread::detail::sched_policy_explicit_bit;

inline bool is_explicit_sched_policy(int native_policy)
{
  return (static_cast<unsigned>(native_policy) & sched_policy_explicit_bit) != 0;
}

inline int to_native_sched_policy(rcpputils::SchedPolicy policy)
{
  return static_cast<unsigned>(policy) & ~sched_policy_explicit_bit;
}

}  // namespace detail
}  // namespace thread
}  // namespace rcpputils

#endif  // RCPPUTILS__THREAD__DETAIL__POSIX__UTILITIES_HPP_
