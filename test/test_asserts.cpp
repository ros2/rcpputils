// Copyright 2020 Amazon.com, Inc. or its affiliates. All Rights Reserved.
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

#include <stdexcept>

#include "gtest/gtest.h"

#include "rcpputils/asserts.hpp"

TEST(test_asserts, check_throws_if_condition_is_false) {
  EXPECT_THROW(rcpputils::check(false), std::runtime_error);
}

TEST(test_asserts, check_does_not_throw_if_condition_is_true) {
  EXPECT_NO_THROW(rcpputils::check(true));
}

#ifndef NDEBUG
TEST(test_asserts, ros_assert_throws_if_condition_is_false_and_ndebug_not_set) {
  EXPECT_THROW(rcpputils::ros_assert(false), std::runtime_error);
}

TEST(test_asserts, ros_assert_does_not_throw_if_condition_is_true_and_ndebug_not_set)
{
  EXPECT_NO_THROW(rcpputils::ros_assert(true));
}

#else
TEST(test_asserts, ros_assert_does_not_throw_if_ndebug_set) {
  EXPECT_NO_THROW(rcpputils::ros_assert(false));
  EXPECT_NO_THROW(rcpputils::ros_assert(true));
}
#endif
