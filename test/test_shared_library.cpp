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

namespace
{

bool endswith(const std::string & basestring, const std::string & substring)
{
  return basestring.length() >= substring.length() && basestring.compare(
    basestring.length() - substring.length(), substring.length(), substring) == 0;
}

}  // namespace

TEST(test_shared_library, valid_load) {
  const std::string library_name = rcpputils::get_platform_library_name("dummy_shared_library");

  try {
    auto library = std::make_shared<rcpputils::SharedLibrary>(library_name);

    EXPECT_TRUE(endswith(library->get_library_path(), library_name)) <<
      "Expected .../" << library_name << ", got " << library->get_library_path();

    EXPECT_TRUE(library->has_symbol("print_name"));

    EXPECT_TRUE(library->has_symbol(std::string("print_name")));

    EXPECT_TRUE(library->get_symbol("print_name") != NULL);

    EXPECT_TRUE(library->get_symbol(std::string("print_name")) != NULL);
  } catch (...) {
    FAIL();
  }
}

TEST(test_shared_library, failed_test) {
  // loading a library that doesn't exists
  std::string library_name = rcpputils::get_platform_library_name("error_library");
  try {
    auto library = std::make_shared<rcpputils::SharedLibrary>(library_name);
    FAIL();
  } catch (...) {
  }

  // Loading a valid library
  library_name = rcpputils::get_platform_library_name("dummy_shared_library");
  try {
    auto library = std::make_shared<rcpputils::SharedLibrary>(library_name);

    // getting and asking for an unvalid symbol
    EXPECT_THROW(library->get_symbol("symbol"), std::runtime_error);
    EXPECT_FALSE(library->has_symbol("symbol"));
  } catch (...) {
    FAIL();
  }

  try {
    auto library = std::make_shared<rcpputils::SharedLibrary>(library_name);

    EXPECT_NO_THROW(library->unload_library());
    EXPECT_THROW(library->unload_library(), std::runtime_error);
  } catch (...) {
    FAIL();
  }
}

TEST(test_shared_library, has_symbol_string_overload) {
  const std::string library_name = rcpputils::get_platform_library_name("dummy_shared_library");
  auto library = std::make_shared<rcpputils::SharedLibrary>(library_name);

  // Positive: string overload delegates to const char* overload.
  EXPECT_TRUE(library->has_symbol(std::string("print_name")));

  // Negative: missing symbol via string overload.
  EXPECT_FALSE(library->has_symbol(std::string("nonexistent_symbol_xyz")));
}

TEST(test_shared_library, get_symbol_string_overload_throws_on_missing) {
  const std::string library_name = rcpputils::get_platform_library_name("dummy_shared_library");
  auto library = std::make_shared<rcpputils::SharedLibrary>(library_name);

  EXPECT_THROW(
    library->get_symbol(std::string("nonexistent_symbol_xyz")),
    std::runtime_error);
}

TEST(test_shared_library, get_library_path_non_empty) {
  const std::string library_name = rcpputils::get_platform_library_name("dummy_shared_library");
  auto library = std::make_shared<rcpputils::SharedLibrary>(library_name);
  EXPECT_FALSE(library->get_library_path().empty());
}

TEST(test_get_platform_library_name, release_name_contains_library_name) {
  const std::string name = rcpputils::get_platform_library_name("mylib", false);
  EXPECT_NE(name.find("mylib"), std::string::npos);
  EXPECT_FALSE(name.empty());
}

TEST(test_get_platform_library_name, debug_and_release_differ_or_equal) {
  // On some platforms (e.g. Windows) debug builds append a 'd' suffix.
  // We only verify both calls succeed and return non-empty strings.
  const std::string release_name = rcpputils::get_platform_library_name("mylib", false);
  const std::string debug_name = rcpputils::get_platform_library_name("mylib", true);
  EXPECT_FALSE(release_name.empty());
  EXPECT_FALSE(debug_name.empty());
}

TEST(test_get_platform_library_name, failed_test) {
  // create a string bigger than the internal buffer
  std::string str(2000, 'A');
  EXPECT_THROW(rcpputils::get_platform_library_name(str), std::runtime_error);
}
