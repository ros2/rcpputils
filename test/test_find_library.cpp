// Copyright 2019 Open Source Robotics Foundation, Inc.
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

#include <stdlib.h>

#include <filesystem>
#include <string>
#include <utility>

#include "gtest/gtest.h"

#include "rcutils/env.h"
#include "rcpputils/find_library.hpp"

namespace
{

std::pair<std::string, std::string> test_lib_path_and_dir()
{
  const char * test_lib_path{};
  EXPECT_EQ(rcutils_get_env("_TEST_LIBRARY", &test_lib_path), nullptr);
  EXPECT_NE(test_lib_path, nullptr);

  const char * test_lib_dir{};
  EXPECT_EQ(rcutils_get_env("_TEST_LIBRARY_DIR", &test_lib_dir), nullptr);
  EXPECT_NE(test_lib_dir, nullptr);

  // Normalize both paths to forward slashes so comparisons are consistent
  // across platforms (e.g. Windows CMake may provide either separator).
  return {
    std::filesystem::path(test_lib_path).generic_string(),
    std::filesystem::path(test_lib_dir).generic_string()
  };
}

#ifdef _WIN32
constexpr const char * kPathVar = "PATH";
constexpr char kPathSep = ';';
#elif __APPLE__
constexpr const char * kPathVar = "DYLD_LIBRARY_PATH";
constexpr char kPathSep = ':';
#else
constexpr const char * kPathVar = "LD_LIBRARY_PATH";
constexpr char kPathSep = ':';
#endif

void set_path_var(const char * value)
{
#ifdef _WIN32
  _putenv_s(kPathVar, value ? value : "");
#else
  if (value) {
    setenv(kPathVar, value, 1);
  } else {
    unsetenv(kPathVar);
  }
#endif
}

}  // anonymous namespace

TEST(test_find_library, find_library)
{
  // Get ground-truth values from CTest properties.
  const auto pair = test_lib_path_and_dir();
  const std::string expected_library_path = pair.first;
  const std::string test_lib_dir = pair.second;

  // Set our relevant path variable.
  const char * env_var{};
#ifdef _WIN32
  env_var = "PATH";
#elif __APPLE__
  env_var = "DYLD_LIBRARY_PATH";
#else
  env_var = "LD_LIBRARY_PATH";
#endif

#ifdef _WIN32
  EXPECT_EQ(_putenv_s(env_var, test_lib_dir.c_str()), 0);
#else
  const int override = 1;
  EXPECT_EQ(setenv(env_var, test_lib_dir.c_str(), override), 0);
#endif

  // Positive test.
  const std::string test_lib_actual = rcpputils::find_library_path("test_library");
  EXPECT_EQ(test_lib_actual, expected_library_path);

  // (Hopefully) Negative test.
  const std::string bad_path = rcpputils::find_library_path(
    "this_is_a_junk_libray_name_please_dont_define_this_if_you_do_then_"
    "you_are_really_naughty");
  EXPECT_EQ(bad_path, "");
}

TEST(test_find_library, find_library_empty_path)
{
  // Empty search path should return empty string and not crash.
  set_path_var("");
  const std::string result = rcpputils::find_library_path("test_library");
  EXPECT_EQ(result, "");
}

TEST(test_find_library, find_library_multiple_dirs_in_path)
{
  // Build a PATH with a dummy leading directory followed by the real one.
  const auto pair = test_lib_path_and_dir();
  const std::string expected_library_path = pair.first;
  const std::string test_lib_dir = pair.second;

  // Prepend a nonexistent directory so the code must iterate past it.
  const std::string nonexistent =
    (std::filesystem::temp_directory_path() / "nonexistent_rcpputils_dir").generic_string();
  const std::string multi_path = nonexistent + kPathSep + test_lib_dir;
  set_path_var(multi_path.c_str());

  const std::string result = rcpputils::find_library_path("test_library");
  EXPECT_EQ(result, expected_library_path);
}

TEST(test_find_library, library_path)
{
  // Get ground-truth values from CTest properties.
  const auto pair = test_lib_path_and_dir();
  const std::string expected_library_path = pair.first;
  const std::string test_lib_dir = pair.second;

  const std::string test_lib_actual = rcpputils::path_for_library(test_lib_dir, "test_library");
  // The returned path must point to the same file as the expected path.
  EXPECT_EQ(test_lib_actual, expected_library_path);

  const std::string bad_path = rcpputils::path_for_library(
    test_lib_dir,
    "highly_unlikely_12_library_34567_name_890.txt.exe");
  EXPECT_EQ(bad_path, "");
}

TEST(test_find_library, path_for_library_nonexistent_dir)
{
  // A directory that does not exist should return empty string.
  const std::string result =
    rcpputils::path_for_library("/no/such/directory", "test_library");
  EXPECT_EQ(result, "");
}

TEST(test_find_library, filename_for_library)
{
  const std::string name = rcpputils::filename_for_library("mylib");
  EXPECT_FALSE(name.empty());
  // The name must contain the library base name.
  EXPECT_NE(name.find("mylib"), std::string::npos);

#ifdef _WIN32
  EXPECT_EQ(name, "mylib.dll");
#elif __APPLE__
  EXPECT_EQ(name, "libmylib.dylib");
#else
  EXPECT_EQ(name, "libmylib.so");
#endif
}

TEST(test_find_library, filename_for_library_empty_name)
{
  // An empty library name should still produce a valid filename (prefix + extension only).
  const std::string name = rcpputils::filename_for_library("");
  EXPECT_FALSE(name.empty());
#ifdef _WIN32
  EXPECT_EQ(name, ".dll");
#elif __APPLE__
  EXPECT_EQ(name, "lib.dylib");
#else
  EXPECT_EQ(name, "lib.so");
#endif
}
