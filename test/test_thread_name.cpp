// Copyright 2025 Open Source Robotics Foundation, Inc.
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

#include "gtest/gtest.h"

#include "rcpputils/thread_name.hpp"

TEST(TestOsThread, set_thread_name) {
  rcpputils::set_thread_name("test_thread");

  EXPECT_STREQ("test_thread", rcpputils::get_thread_name().c_str());
}

#if defined(_WIN32)
TEST(TestOsThread, no_truncation_on_windows) {
  rcpputils::set_thread_name("0123456789abcdef");

  // Should be unaffected by truncation
  EXPECT_STREQ("0123456789abcdef", rcpputils::get_thread_name().c_str());
}
#elif defined(__APPLE__)
TEST(TestOsThread, truncation_on_apple) {
  rcpputils::set_thread_name(
    "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef");

  // Should be 63 characters and then the null terminator
  EXPECT_STREQ("0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcde",
    rcpputils::get_thread_name().c_str());
}
#else  // posix
TEST(TestOsThread, truncation_on_posix) {
  rcpputils::set_thread_name("0123456789abcdef");

  // Should be 15 characters and then the null terminator
  EXPECT_STREQ("0123456789abcde", rcpputils::get_thread_name().c_str());
}
#endif
