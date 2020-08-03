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

#include "gtest/gtest.h"

#include "rcutils/allocator.h"
#include "rcutils/process.h"
#include "rcutils/strdup.h"

#include "rcpputils/patch.hpp"
#include "rcpputils/process.hpp"

RCPPUTILS_PATCH_IGNORE_OPERATOR(rcutils_allocator_t, ==)
RCPPUTILS_PATCH_IGNORE_OPERATOR(rcutils_allocator_t, !=)
RCPPUTILS_PATCH_IGNORE_OPERATOR(rcutils_allocator_t, <)
RCPPUTILS_PATCH_IGNORE_OPERATOR(rcutils_allocator_t, >)

TEST(TestPatch, test_patched_get_executable_name) {
  {
    auto p = rcpputils::patch(
      "self", rcutils_get_executable_name,
      [](rcutils_allocator_t allocator) {
        return rcutils_strdup("dummy_name", allocator);
      });
    EXPECT_EQ("dummy_name", rcpputils::get_executable_name());
  }

  EXPECT_EQ("test_patch", rcpputils::get_executable_name());
}
