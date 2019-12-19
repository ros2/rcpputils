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

#include <gtest/gtest.h>

#include <fstream>
#include <string>

#include "rcpputils/filesystem_helper.hpp"
#include "temporary_directory_fixture.hpp"

#ifdef _WIN32
static constexpr const bool is_win32 = true;
#else
static constexpr const bool is_win32 = false;
#endif

namespace
{
/**
 * Create a file with a specific size specified in MiB.
 *
 * @param uri Path to the file to create.
 * @param size_in_mib File size in MiB
 */
void create_file(const std::string & uri, int size_in_mib)
{
  std::ofstream out{uri};
  out.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  const std::string file_text{"test"};
  const auto size_in_bytes = size_in_mib * 1024 * 1024;
  const auto num_iterations = size_in_bytes / static_cast<int>(file_text.size());

  for (auto i = 0; i < num_iterations; ++i) {
    out << file_text;
  }
}
}  // namespace

using path = rcpputils::fs::path;

std::string build_extension_path()
{
  return is_win32 ? R"(C:\foo\bar\baz.yml)" : "/bar/foo/baz.yml";
}

std::string build_double_extension_path()
{
  return is_win32 ? R"(C:\bar\baz.bar.yml)" : "/foo/baz.bar.yml";
}

std::string build_no_extension_path()
{
  return is_win32 ? R"(C:\bar\baz)" : "/bar/baz";
}

std::string build_directory_path()
{
  return is_win32 ? R"(.\test_folder)" : R"(./test_folder)";
}

TEST(TestFilesystemHelper, join_path)
{
  auto p = path("foo") / path("bar");

  if (is_win32) {
    EXPECT_EQ("foo\\bar", p.string());
  } else {
    EXPECT_EQ("foo/bar", p.string());
  }
}

TEST(TestFilesystemHelper, to_native_path)
{
  {
    auto p = path("/foo/bar/baz");
    if (is_win32) {
      EXPECT_EQ("\\foo\\bar\\baz", p.string());
    } else {
      EXPECT_EQ("/foo/bar/baz", p.string());
    }
  }
  {
    auto p = path("\\foo\\bar\\baz");
    if (is_win32) {
      EXPECT_EQ("\\foo\\bar\\baz", p.string());
    } else {
      EXPECT_EQ("/foo/bar/baz", p.string());
    }
  }
  {
    auto p = path("/foo//bar/baz");
    if (is_win32) {
      EXPECT_EQ("\\foo\\\\bar\\baz", p.string());
    } else {
      EXPECT_EQ("/foo//bar/baz", p.string());
    }
  }
  {
    auto p = path("\\foo\\\\bar\\baz");
    if (is_win32) {
      EXPECT_EQ("\\foo\\\\bar\\baz", p.string());
    } else {
      EXPECT_EQ("/foo//bar/baz", p.string());
    }
  }
}

TEST(TestFilesystemHelper, is_absolute)
{
  if (is_win32) {
    {
      auto p = path("C:\\foo\\bar\\baz");
      EXPECT_TRUE(p.is_absolute());
    }
    {
      auto p = path("D:\\foo\\bar\\baz");
      EXPECT_TRUE(p.is_absolute());
    }
    {
      auto p = path("C:/foo/bar/baz");
      EXPECT_TRUE(p.is_absolute());
    }
    {
      auto p = path("foo/bar/baz");
      EXPECT_FALSE(p.is_absolute());
    }
  } else {
    {
      auto p = path("/foo/bar/baz");
      EXPECT_TRUE(p.is_absolute());
    }
    {
      auto p = path("foo/bar/baz");
      EXPECT_FALSE(p.is_absolute());
    }
  }
}

TEST(TestFilesystemHelper, correct_extension)
{
  {
    auto p = path(build_extension_path());
    auto ext = p.extension();
    EXPECT_EQ(".yml", ext.string());
  }
  {
    auto p = path(build_double_extension_path());
    auto ext = p.extension();
    EXPECT_EQ(".yml", ext.string());
  }
  {
    auto p = path(build_no_extension_path());
    auto ext = p.extension();
    EXPECT_EQ("", ext.string());
  }
}

TEST(TestFilesystemHelper, is_empty)
{
  auto p = path("");
  EXPECT_TRUE(p.empty());
}

/**
 * Test create directories.
 *
 * NOTE: expects the current directory to be write-only, else test will fail.
 *
 */
TEST(TestFilesystemHelper, create_directories)
{
  auto p = path(build_directory_path());
  EXPECT_TRUE(rcpputils::fs::create_directories(p));
}

TEST(TestFilesystemHelper, remove_extension)
{
  auto p = path("foo.txt");
  p = rcpputils::fs::remove_extension(p.string());
  EXPECT_EQ("foo", p.string());
}

TEST(TestFilesystemHelper, remove_extensions)
{
  auto p = path("foo.txt.compress");
  p = rcpputils::fs::remove_extension(p, 2);
  EXPECT_EQ("foo", p.string());
}

TEST(TestFilesystemHelper, remove_extensions_overcount)
{
  auto p = path("foo.txt.compress");
  p = rcpputils::fs::remove_extension(p, 4);
  EXPECT_EQ("foo", p.string());
}

TEST(TestFilesystemHelper, remove_extension_no_extension)
{
  auto p = path("foo");
  p = rcpputils::fs::remove_extension(p);
  EXPECT_EQ("foo", p.string());
}

class FilesystemHelperFixture : public TemporaryDirectoryFixture {};

TEST_F(FilesystemHelperFixture, get_file_size)
{
  constexpr const std::uintmax_t expected_file_size_mib = 1;
  const rcpputils::fs::path file_name{"file1.txt"};
  const auto uri = path{temporary_dir_path_} / file_name;
  create_file(uri.string(), expected_file_size_mib);

  const auto file_size = rcpputils::fs::file_size(uri);
  const std::uintmax_t expected_file_size_bytes = expected_file_size_mib * 1024 * 1024;
  EXPECT_EQ(file_size, expected_file_size_bytes);
}

TEST_F(FilesystemHelperFixture, get_file_size_file_does_not_exist)
{
  const rcpputils::fs::path file_name{"file1.txt"};
  const auto uri = path{temporary_dir_path_} / file_name;
  EXPECT_THROW(
    {rcpputils::fs::file_size(uri);},
    std::system_error
  );
}
