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

#include <string>

#include "rcpputils/filesystem_helper.hpp"

#ifdef WIN32
static constexpr const bool is_win32 = true;
#else
static constexpr const bool is_win32 = false;
#endif

using path = rcpputils::fs::path;

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
  if (is_win32) {
    {
      auto p = path("C:\\foo\\bar\\baz.yml");
      auto ext = p.extension();
      EXPECT_EQ(".yml", ext);
    }
    {
      auto p = path("C:\\foo\\baz.bar.yml");
      auto ext = p.extension();
      EXPECT_EQ(".yml", ext);
    }
  } else {
    {
      auto p = path("/foo/bar/baz.yml");
      auto ext = p.extension();
      EXPECT_EQ(".yml", ext);
    }
    {
      auto p = path("/foo/baz.bar.yml");
      auto ext = p.extension();
      EXPECT_EQ(".yml", ext);
    }
  }
}

TEST(TestFilesystemHelper, is_empty)
{
  {
    auto p = path("");
    EXPECT_TRUE(p.empty());
  }
}

TEST(TestFilesystemHelper, create_directories)
{
  if (is_win32) {
    {
      auto p = path(".\\test_folder");
      EXPECT_TRUE(rcpputils::fs::create_directories(p));
    }
  } else {
    {
      auto p = path("./test_folder");
      EXPECT_TRUE(rcpputils::fs::create_directories(p));
    }
  }
}
