// Copyright 2020 Open Source Robotics Foundation, Inc.
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

#ifndef RCPPUTILS__PATCH_HPP_
#define RCPPUTILS__PATCH_HPP_

#include <functional>
#include <string>
#include <utility>

#include "mimick/mimick.h"
#include "rcutils/macros.h"

namespace rcpputils
{

template<size_t N, typename SignatureT>
struct patch_traits;

template<size_t N, typename ReturnType>
struct patch_traits<N, ReturnType(void)>
{
  mmk_mock_define(mock_type, ReturnType);
};

template<size_t N, typename ReturnType, typename ArgType0>
struct patch_traits<N, ReturnType(ArgType0)>
{
  mmk_mock_define(mock_type, ReturnType, ArgType0);
};

template<size_t N, typename ReturnType,
  typename ArgType0, typename ArgType1>
struct patch_traits<N, ReturnType(ArgType0, ArgType1)>
{
  mmk_mock_define(mock_type, ReturnType, ArgType0, ArgType1);
};

template<size_t N, typename ReturnType,
  typename ArgType0, typename ArgType1, typename ArgType2>
struct patch_traits<N, ReturnType(ArgType0, ArgType1, ArgType2)>
{
  mmk_mock_define(mock_type, ReturnType, ArgType0, ArgType1, ArgType2);
};

template<size_t N, typename ReturnType,
  typename ArgType0, typename ArgType1,
  typename ArgType2, typename ArgType3>
struct patch_traits<N, ReturnType(ArgType0, ArgType1, ArgType2, ArgType3)>
{
  mmk_mock_define(mock_type, ReturnType, ArgType0, ArgType1, ArgType2, ArgType3);
};

template<size_t N, typename ReturnType,
  typename ArgType0, typename ArgType1,
  typename ArgType2, typename ArgType3, typename ArgType4>
struct patch_traits<N, ReturnType(ArgType0, ArgType1, ArgType2, ArgType3, ArgType4)>
{
  mmk_mock_define(mock_type, ReturnType, ArgType0, ArgType1, ArgType2, ArgType3, ArgType4);
};

template<size_t N, typename ReturnType,
  typename ArgType0, typename ArgType1,
  typename ArgType2, typename ArgType3,
  typename ArgType4, typename ArgType5>
struct patch_traits<N, ReturnType(ArgType0, ArgType1, ArgType2, ArgType3, ArgType4, ArgType5)>
{
  mmk_mock_define(
    mock_type, ReturnType, ArgType0, ArgType1, ArgType2, ArgType3, ArgType4, ArgType5);
};

template<size_t N, typename SignatureT>
struct trampoline;

template<size_t N, typename ReturnT, typename ... ArgTs>
struct trampoline<N, ReturnT(ArgTs...)>
{
  static ReturnT base(ArgTs... args)
  {
    return target(std::forward<ArgTs>(args)...);
  }

  static std::function<ReturnT(ArgTs...)> target;
};

template<size_t N, typename ReturnT, typename ... ArgTs>
std::function<ReturnT(ArgTs...)>
trampoline<N, ReturnT(ArgTs...)>::target;

template<size_t N, typename SignatureT>
auto prepare_trampoline(std::function<SignatureT> target)
{
  trampoline<N, SignatureT>::target = target;
  return trampoline<N, SignatureT>::base;
}

template<size_t N, typename SignatureT>
class patch;

template<size_t N, typename ReturnT, typename ... ArgTs>
class patch<N, ReturnT(ArgTs...)>
{
public:
  using mock_type = typename patch_traits<N, ReturnT(ArgTs...)>::mock_type;

  patch(const std::string & target, std::function<ReturnT(ArgTs...)> proxy)
  : proxy_(proxy)
  {
    auto MMK_MANGLE(mock_type, create) =
      patch_traits<N, ReturnT(ArgTs...)>::MMK_MANGLE(mock_type, create);
    mock_ = mmk_mock(target.c_str(), mock_type);
  }

  patch(const patch &) = delete;
  patch & operator=(const patch &) = delete;

  patch(patch && other)
  {
    mock_ = other.mock_;
    other.mock_ = nullptr;
  }

  patch & operator=(patch && other)
  {
    if (mock_) {
      mmk_reset(mock_);
    }
    mock_ = other.mock_;
    other.mock_ = nullptr;
  }

  ~patch()
  {
    if (mock_) {
      mmk_reset(mock_);
    }
  }

  patch & then_call(std::function<ReturnT(ArgTs...)> replacement) &
  {
    auto type_erased_trampoline =
      reinterpret_cast<mmk_fn>(prepare_trampoline<N>(replacement));
    mmk_when(proxy_(any<ArgTs>()...), .then_call = type_erased_trampoline);
    return *this;
  }

  patch && then_call(std::function<ReturnT(ArgTs...)> replacement) &&
  {
    auto type_erased_trampoline =
      reinterpret_cast<mmk_fn>(prepare_trampoline<N>(replacement));
    mmk_when(proxy_(any<ArgTs>()...), .then_call = type_erased_trampoline);
    return std::move(*this);
  }

private:
  template<typename T>
  T any() {return mmk_any(T);}

  mock_type mock_;
  std::function<ReturnT(ArgTs...)> proxy_;
};

template<size_t N, typename SignatureT>
auto make_patch(const std::string & target, std::function<SignatureT> proxy)
{
  return patch<N, SignatureT>(target, proxy);
}

}  // namespace rcpputils

#define RCPPUTILS_PATCH_IGNORE_OPERATOR(type, op) \
  bool operator op(const type &, const type &) { \
    return false; \
  }

#define RCPPUTILS_PATCH_PROXY(func) \
  [] (auto && ... args)->decltype(auto) { \
    return func(std::forward<decltype(args)>(args)...); \
  }

#define RCPPUTILS_PATCH_TARGET(what, where) \
  (std::string(RCUTILS_STRINGIFY(where)) + "@" + (what))

#define patch(what, where, with) \
  make_patch<__COUNTER__, decltype(where)>( \
    RCPPUTILS_PATCH_TARGET(what, where), RCPPUTILS_PATCH_PROXY(where) \
  ).then_call(with)

#endif  // RCPPUTILS__PATCH_HPP_
