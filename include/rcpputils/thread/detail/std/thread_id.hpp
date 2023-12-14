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

#ifndef RCPPUTILS__THREAD__DETAIL__STD__THREAD_ID_HPP_
#define RCPPUTILS__THREAD__DETAIL__STD__THREAD_ID_HPP_

#include <thread>

namespace rcpputils
{

namespace thread
{

namespace detail
{

using NativeIdType = std::thread::id;

inline bool id_equal(NativeIdType id1, NativeIdType id2)
{
  return id1 == id2;
}

inline NativeIdType get_native_thread_id() noexcept
{
  return std::this_thread::get_id();
}

}  // namespace detail

}  // namespace thread

}  // namespace rcpputils

#endif  // RCPPUTILS__THREAD__DETAIL__STD__THREAD_ID_HPP_
