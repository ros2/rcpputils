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

#ifndef RCPPUTILS__THREADS__POSIX__THREAD_FUNC_HPP_
#define RCPPUTILS__THREADS__POSIX__THREAD_FUNC_HPP_

#include <memory>
#include <type_traits>
#include <utility>

namespace rcpputils::detail
{

struct ThreadFuncBase
{
  virtual ~ThreadFuncBase() = default;
  virtual void run() = 0;
};

template<typename F>
struct ThreadFunc : ThreadFuncBase
{
  template<typename G>
  explicit ThreadFunc(G && g)
  : func_(std::forward<G>(g))
  {}

private:
  void run() override
  {
    func_();
  }

  F func_;
};

template<typename F>
ThreadFunc(F &&)->ThreadFunc<std::decay_t<F>>;

}  // namespace rcpputils::detail

#endif  // RCPPUTILS__THREADS__POSIX__THREAD_FUNC_HPP_
