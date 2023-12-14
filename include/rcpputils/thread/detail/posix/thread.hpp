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

#ifndef RCPPUTILS__THREAD__DETAIL__POSIX__THREAD_HPP_
#define RCPPUTILS__THREAD__DETAIL__POSIX__THREAD_HPP_

#include <pthread.h>
#include <unistd.h>
#include <condition_variable>
#include <cstdio>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <system_error>
#include <type_traits>
#include <utility>

#include "rcpputils/thread/detail/thread_id.hpp"
#include "rcpputils/thread/detail/posix/thread_attribute.hpp"
#include "rcpputils/thread/detail/posix/thread_func.hpp"
#include "rcpputils/thread/detail/posix/utilities.hpp"

#include "rcpputils/visibility_control.hpp"

namespace rcpputils
{

RCPPUTILS_PUBLIC_TYPE
struct Thread
{
  using NativeHandleType = pthread_t;
  using Attribute = ThreadAttribute;
  using Id = ThreadId;

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
    // Assume pthread_t is an invalid handle if it's 0
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
  }

  void join()
  {
    void * p;
    int r = pthread_join(handle_, &p);
    thread::detail::throw_if_error(r, "error in pthread_join");
    handle_ = NativeHandleType{};
  }

  bool joinable() const noexcept
  {
    return 0 == pthread_equal(handle_, NativeHandleType{});
  }

  void detach()
  {
    int r = pthread_detach(handle_);
    thread::detail::throw_if_error(r, "error in pthread_detach");
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
  using ThreadFuncBase = thread::detail::ThreadFuncBase;
  template<typename F, typename ... Args>
  static std::unique_ptr<ThreadFuncBase> make_thread_func(F && f, Args && ... args)
  {
    using thread::detail::ThreadFunc;

    static_assert(
      !std::is_member_object_pointer_v<std::decay_t<F>>,
      "F is a pointer to member, that has no effect on a thread");

    ThreadFuncBase * func = new ThreadFunc(std::forward<F>(f), std::forward<Args>(args)...);
    return std::unique_ptr<ThreadFuncBase>(func);
  }
  template<typename F, typename ... Args>
  static std::unique_ptr<ThreadFuncBase> make_thread_func_with_attr(
    Attribute const & attr,
    F && f,
    Args && ... args)
  {
    using thread::detail::ThreadFunc;

    static_assert(
      !std::is_member_object_pointer_v<std::decay_t<F>>,
      "F is a pointer to member, that has no effect on a thread");

    ThreadFuncBase * func = new ThreadFunc(
      [](F & f, Attribute & attr, Args & ... args)
      {
        apply_attr(attr);
        std::invoke(f, args ...);
      }, std::forward<F>(f), attr, std::forward<Args>(args)...);
    return std::unique_ptr<ThreadFuncBase>(func);
  }

  Thread(Attribute const * attr, std::unique_ptr<ThreadFuncBase> func);

  static void apply_attr(Attribute const & attr);

  NativeHandleType handle_;
};

inline void swap(Thread & t1, Thread & t2)
{
  t1.swap(t2);
}

namespace this_thread
{

inline void yield() noexcept
{
  sched_yield();
}

}  // namespace this_thread

}  // namespace rcpputils

#endif  // RCPPUTILS__THREAD__DETAIL__POSIX__THREAD_HPP_
