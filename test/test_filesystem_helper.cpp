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

using path = rcpputils::fs::path;

TEST(TestFilesystemHelper, join_path)
{
  auto p = path("foo") / path("bar");

#ifdef _WIN32
  const char * ref_str = "foo\\bar";
#else
  const char * ref_str = "foo/bar";
#endif  // _WIN32

  EXPECT_STREQ(ref_str, p.string().c_str());
}

TEST(TestFilesystemHelper, to_native_path)
{
  {
    auto p = path("/foo/bar/baz");
#ifdef _WIN32
    const char * ref_str = "\\foo\\bar\\baz";
#else
    const char * ref_str = "/foo/bar/baz";
#endif  // _WIN32
    EXPECT_STREQ(ref_str, p.string().c_str());
  }
  {
    auto p = path("/foo//bar/baz");
#ifdef _WIN32
    const char * ref_str = "\\foo\\\\bar\\baz";
#else
    const char * ref_str = "/foo//bar/baz";
#endif  // _WIN32
    EXPECT_STREQ(ref_str, p.string().c_str());
  }
}
