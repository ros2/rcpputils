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

#include <gtest/gtest.h>

#include <memory>
#include <string>

#include "rcpputils/shared_library.hpp"

TEST(test_shared_library, valid_load) {
  // library path
  const std::string library_path = std::string("libdummy_shared_library.so");

  EXPECT_NO_THROW(std::make_shared<rcpputils::SharedLibrary>(library_path));
  EXPECT_ANY_THROW(std::make_shared<rcpputils::SharedLibrary>("library_path"));

  auto library = std::make_shared<rcpputils::SharedLibrary>(library_path);

  EXPECT_TRUE(library->has_symbol("print_name"));
  EXPECT_FALSE(library->has_symbol("symbol"));

  EXPECT_TRUE(library->get_symbol("print_name") != NULL);
  EXPECT_ANY_THROW(library->get_symbol("symbol"));
}
