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

#ifndef RCPPUTILS__THREADS__POSIX__THREAD_HPP_
#define RCPPUTILS__THREADS__POSIX__THREAD_HPP_

#include <pthread.h>
#include <unistd.h>
#include <condition_variable>
#include <cstdio>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <system_error>
#include <tuple>
#include <type_traits>
#include <utility>

#include "rcpputils/threads/posix/thread_attribute.hpp"
#include "rcpputils/threads/posix/thread_func.hpp"
#include "rcpputils/threads/posix/thread_id.hpp"
#include "rcpputils/threads/posix/utilities.hpp"
#include "rcpputils/visibility_control.hpp"

namespace rcpputils
{

RCPPUTILS_PUBLIC_TYPE
struct Thread
{
  using NativeHandleType = pthread_t;
  using Attribute = detail::ThreadAttribute;
  using Id = detail::ThreadId;

  // Assume pthread_t is an invalid handle if it's 0
  Thread() noexcept
  : handle_{} {}
  Thread(Thread && other)
  : handle_{}
  {
    swap(other);
  }
  template<typename F, typename ... Args,
    typename = std::enable_if_t<!std::is_same<std::decay_t<F>, Attribute>::value>>
  explicit Thread(F && f, Args && ... args)
  : Thread(
      static_cast<Attribute const *>(nullptr),
      make_thread_func(std::forward<F>(f), std::forward<Args>(args)...))
  {}
  template<typename F, typename ... Args>
  Thread(Attribute const & attr, F && f, Args && ... args)
  : Thread(
      &attr,
      make_thread_func_with_attr(attr, std::forward<F>(f), std::forward<Args>(args)...))
  {}
  Thread(Thread const &) = delete;
  ~Thread()
  {
    if (handle_) {
      std::terminate();
    }
  }

  Thread & operator=(Thread && other) noexcept
  {
    if (handle_) {
      std::terminate();
    }
    swap(other);
    return *this;
  }

  Thread & operator=(Thread const &) = delete;

  void swap(Thread & other)
  {
    using std::swap;
    swap(handle_, other.handle_);
    swap(name_, other.name_);
  }

  void join()
  {
    void * p;
    int r = pthread_join(handle_, &p);
    detail::throw_if_error(r, "Error in pthread_join ");
    handle_ = NativeHandleType{};
  }

  bool joinable() const noexcept
  {
    return 0 == pthread_equal(handle_, NativeHandleType{});
  }

  void detach()
  {
    int r = pthread_detach(handle_);
    detail::throw_if_error(r, "Error in pthread_detach ");
    handle_ = NativeHandleType{};
  }

  NativeHandleType native_handle() const
  {
    return handle_;
  }

  Id get_id() const noexcept
  {
    return Id{handle_};
  }

  static unsigned int hardware_concurrency() noexcept
  {
    auto r = sysconf(_SC_NPROCESSORS_ONLN);
    if (r == -1) {
      return 0u;
    } else {
      return static_cast<unsigned int>(r);
    }
  }

private:
  using ThreadFuncBase = detail::ThreadFuncBase;
  template<typename F, typename ... Args>
  static std::unique_ptr<ThreadFuncBase> make_thread_func(F && f, Args && ... args)
  {
    static_assert(
      !std::is_member_object_pointer_v<std::decay_t<F>>,
      "F is a pointer to member, that has no effect on a thread");

    detail::ThreadFuncBase * func = new detail::ThreadFunc(
      [f = std::forward<F>(f), args = std::tuple(std::forward<Args>(args)...)]() mutable
      {
        std::apply(f, args);
      });
    return std::unique_ptr<ThreadFuncBase>(func);
  }
  template<typename F, typename ... Args>
  static std::unique_ptr<ThreadFuncBase> make_thread_func_with_attr(
    Attribute const & attr,
    F && f,
    Args && ... args)
  {
    static_assert(
      !std::is_member_object_pointer_v<std::decay_t<F>>,
      "F is a pointer to member, that has no effect on a thread");

    detail::ThreadFuncBase * func = new detail::ThreadFunc(
      [attr, f = std::forward<F>(f), args = std::tuple(std::forward<Args>(args)...)]() mutable
      {
        std::apply(f, args);
      });
    return std::unique_ptr<ThreadFuncBase>(func);
  }

  Thread(Attribute const * attr, std::unique_ptr<ThreadFuncBase> func);

  static void apply_attr(Attribute const & attr);

  NativeHandleType handle_;
  std::string name_;
};

inline void swap(Thread & t1, Thread & t2)
{
  t1.swap(t2);
}

namespace detail
{
void apply_attr_to_current_thread(ThreadAttribute const & attr);
}

namespace this_thread
{

template<typename F, typename ... Args>
void run_with_thread_attribute(
  detail::ThreadAttribute const & attr, F && f, Args && ... args)
{
  static_assert(
    !std::is_member_object_pointer_v<std::decay_t<F>>,
    "F is a pointer to member, that has no effect on a thread");

  detail::apply_attr_to_current_thread(attr);
  std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
}

inline void yield() noexcept
{
  sched_yield();
}

}  // namespace this_thread

}  // namespace rcpputils

#endif  // RCPPUTILS__THREADS__POSIX__THREAD_HPP_
