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

#ifndef RCPPUTILS__THREAD__DETAIL__THREAD_ID_HPP_
#define RCPPUTILS__THREAD__DETAIL__THREAD_ID_HPP_

#include <thread>

#if __linux__
#include "rcpputils/thread/detail/posix/thread_id.hpp"
#else
#include "rcpputils/thread/detail/std/thread_id.hpp"
#endif

namespace rcpputils
{

struct ThreadId;

inline bool operator==(ThreadId id1, ThreadId id2);
inline bool operator!=(ThreadId id1, ThreadId id2);
inline bool operator<(ThreadId id1, ThreadId id2);
inline bool operator>(ThreadId id1, ThreadId id2);
inline bool operator<=(ThreadId id1, ThreadId id2);
inline bool operator>=(ThreadId id1, ThreadId id2);
template<typename CharT, typename Traits>
inline std::basic_ostream<CharT, Traits> & operator<<(
  std::basic_ostream<CharT, Traits> &,
  ThreadId);

namespace this_thread
{
inline ThreadId get_id() noexcept;
}

struct ThreadId
{
  ThreadId() = default;
  ThreadId(ThreadId const &) = default;
  ThreadId(ThreadId &&) = default;
  ThreadId & operator=(ThreadId const &) = default;
  ThreadId & operator=(ThreadId &&) = default;

  friend bool operator==(ThreadId id1, ThreadId id2)
  {
    return thread::detail::id_equal(id1.h, id2.h);
  }
  friend bool operator<(ThreadId id1, ThreadId id2)
  {
    return id1.h < id2.h;
  }
  template<typename CharT, typename Traits>
  friend std::basic_ostream<CharT, Traits> & operator<<(
    std::basic_ostream<CharT, Traits> & ost,
    ThreadId id)
  {
    return ost << id.h;
  }

private:
  friend class Thread;
  friend ThreadId this_thread::get_id() noexcept;
  friend struct std::hash<ThreadId>;
  explicit ThreadId(thread::detail::NativeIdType h)
  : h(h) {}
  thread::detail::NativeIdType h;
};

bool operator!=(ThreadId id1, ThreadId id2)
{
  return !(id1 == id2);
}

bool operator>(ThreadId id1, ThreadId id2)
{
  return id2 < id1;
}

bool operator<=(ThreadId id1, ThreadId id2)
{
  return !(id1 > id2);
}

bool operator>=(ThreadId id1, ThreadId id2)
{
  return !(id1 < id2);
}

namespace this_thread
{

inline ThreadId get_id() noexcept
{
  return ThreadId{thread::detail::get_native_thread_id()};
}

}  // namespace this_thread

}  // namespace rcpputils

namespace std
{

template<>
struct hash<rcpputils::ThreadId>
  : hash<rcpputils::thread::detail::NativeIdType>
{};

}  // namespace std

#endif  // RCPPUTILS__THREAD__DETAIL__THREAD_ID_HPP_
