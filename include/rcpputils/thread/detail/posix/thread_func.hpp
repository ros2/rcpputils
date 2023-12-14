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

#ifndef RCPPUTILS__THREAD__DETAIL__POSIX__THREAD_FUNC_HPP_
#define RCPPUTILS__THREAD__DETAIL__POSIX__THREAD_FUNC_HPP_

#include <type_traits>
#include <utility>

namespace rcpputils
{
namespace thread
{
namespace detail
{

template<std::size_t, typename Arg>
struct ThreadArg
{
  Arg arg_;
};

struct ThreadFuncBase
{
  virtual ~ThreadFuncBase() = default;
  virtual void run() = 0;
};

template<typename F, typename Is, typename ... Args>
struct ThreadFunc;
template<typename F, std::size_t ... Is, typename ... Args>
struct ThreadFunc<F, std::index_sequence<Is...>, Args...>
  : ThreadFuncBase, private ThreadArg<Is, Args>...
{
  template<typename G, typename ... As>
  explicit ThreadFunc(G && g, As && ... args)
  : ThreadArg<Is, Args>{std::forward<As>(args)}..., func_{std::forward<G>(g)}
  {}

private:
  void run() override
  {
    std::invoke(func_, ThreadArg<Is, Args>::arg_ ...);
  }

  F func_;
};

template<typename F, typename ... Args>
ThreadFunc(F &&, Args && ...)->ThreadFunc<
  std::decay_t<F>, std::index_sequence_for<Args...>, std::decay_t<Args>...>;

}  // namespace detail
}  // namespace thread
}  // namespace rcpputils

#endif  // RCPPUTILS__THREAD__DETAIL__POSIX__THREAD_FUNC_HPP_
