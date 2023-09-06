// Copyright 2023 Open Source Robotics Foundation, Inc.
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

#include <gtest/gtest.h>

#include <rcpputils/unique_lock.hpp>

TEST(test_time, test_compile_multiple_mutex_types) {
  // Very simple check that this compiles with different mutex types
  std::mutex regular_mutex;
  rcpputils::unique_lock<std::mutex> lock1(regular_mutex);

  std::recursive_mutex recursive_mutex;
  rcpputils::unique_lock<std::recursive_mutex> lock2(recursive_mutex);
}
