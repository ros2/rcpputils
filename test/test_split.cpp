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

#include "rcpputils/split.hpp"

TEST(test_split, split) {
  {
    auto ret = rcpputils::split("", '/', true);
    EXPECT_EQ(0u, ret.size());
  }
  {
    auto ret = rcpputils::split("", '/', false);
    EXPECT_EQ(0u, ret.size());
  }
  {
    auto ret = rcpputils::split("hello_world", '/', true);
    EXPECT_EQ(1u, ret.size());
    EXPECT_EQ("hello_world", ret[0]);
  }
  {
    auto ret = rcpputils::split("hello_world", '/', false);
    EXPECT_EQ(1u, ret.size());
    EXPECT_EQ("hello_world", ret[0]);
  }
  {
    auto ret = rcpputils::split("hello/world", '/', true);
    EXPECT_EQ(2u, ret.size());
    EXPECT_EQ("hello", ret[0]);
    EXPECT_EQ("world", ret[1]);
  }
  {
    auto ret = rcpputils::split("hello/world", '/', false);
    EXPECT_EQ(2u, ret.size());
    EXPECT_EQ("hello", ret[0]);
    EXPECT_EQ("world", ret[1]);
  }
  {
    auto ret = rcpputils::split("/hello/world", '/', true);
    EXPECT_EQ(2u, ret.size());
    EXPECT_EQ("hello", ret[0]);
    EXPECT_EQ("world", ret[1]);
  }
  {
    auto ret = rcpputils::split("/hello/world", '/', false);
    EXPECT_EQ(3u, ret.size());
    EXPECT_EQ("", ret[0]);
    EXPECT_EQ("hello", ret[1]);
    EXPECT_EQ("world", ret[2]);
  }
  {
    auto ret = rcpputils::split("hello/world/", '/', true);
    EXPECT_EQ(2u, ret.size());
    EXPECT_EQ("hello", ret[0]);
    EXPECT_EQ("world", ret[1]);
  }
  {
    auto ret = rcpputils::split("hello/world/", '/', false);
    EXPECT_EQ(2u, ret.size());
    EXPECT_EQ("hello", ret[0]);
    EXPECT_EQ("world", ret[1]);
  }
  {
    auto ret = rcpputils::split("hello//world", '/', true);
    EXPECT_EQ(2u, ret.size());
    EXPECT_EQ("hello", ret[0]);
    EXPECT_EQ("world", ret[1]);
  }
  {
    auto ret = rcpputils::split("hello//world", '/', false);
    EXPECT_EQ(3u, ret.size());
    EXPECT_EQ("hello", ret[0]);
    EXPECT_EQ("", ret[1]);
    EXPECT_EQ("world", ret[2]);
  }
  {
    auto ret = rcpputils::split("/my/hello/world", '/', true);
    EXPECT_EQ(3u, ret.size());
    EXPECT_EQ("my", ret[0]);
    EXPECT_EQ("hello", ret[1]);
    EXPECT_EQ("world", ret[2]);
  }
  {
    auto ret = rcpputils::split("/my/hello/world", '/', false);
    EXPECT_EQ(4u, ret.size());
    EXPECT_EQ("", ret[0]);
    EXPECT_EQ("my", ret[1]);
    EXPECT_EQ("hello", ret[2]);
    EXPECT_EQ("world", ret[3]);
  }
  {
    auto ret = rcpputils::split("/my//hello//world/", '/', true);
    EXPECT_EQ(3u, ret.size());
    EXPECT_EQ("my", ret[0]);
    EXPECT_EQ("hello", ret[1]);
    EXPECT_EQ("world", ret[2]);
  }
  {
    auto ret = rcpputils::split("/my//hello//world/", '/', false);
    EXPECT_EQ(6u, ret.size());
    EXPECT_EQ("", ret[0]);
    EXPECT_EQ("my", ret[1]);
    EXPECT_EQ("", ret[2]);
    EXPECT_EQ("hello", ret[3]);
    EXPECT_EQ("", ret[4]);
    EXPECT_EQ("world", ret[5]);
  }
}

TEST(test_split, split_backslash) {
  {
    auto ret = rcpputils::split("", '\\', true);
    EXPECT_EQ(0u, ret.size());
  }
  {
    auto ret = rcpputils::split("", '\\', false);
    EXPECT_EQ(0u, ret.size());
  }
  {
    auto ret = rcpputils::split("hello_world", '\\', true);
    EXPECT_EQ(1u, ret.size());
    EXPECT_EQ("hello_world", ret[0]);
  }
  {
    auto ret = rcpputils::split("hello_world", '\\', false);
    EXPECT_EQ(1u, ret.size());
    EXPECT_EQ("hello_world", ret[0]);
  }
  {
    auto ret = rcpputils::split("hello\\world", '\\', true);
    EXPECT_EQ(2u, ret.size());
    EXPECT_EQ("hello", ret[0]);
    EXPECT_EQ("world", ret[1]);
  }
  {
    auto ret = rcpputils::split("hello\\world", '\\', false);
    EXPECT_EQ(2u, ret.size());
    EXPECT_EQ("hello", ret[0]);
    EXPECT_EQ("world", ret[1]);
  }
  {
    auto ret = rcpputils::split("\\hello\\world", '\\', true);
    EXPECT_EQ(2u, ret.size());
    EXPECT_EQ("hello", ret[0]);
    EXPECT_EQ("world", ret[1]);
  }
  {
    auto ret = rcpputils::split("\\hello\\world", '\\', false);
    EXPECT_EQ(3u, ret.size());
    EXPECT_EQ("", ret[0]);
    EXPECT_EQ("hello", ret[1]);
    EXPECT_EQ("world", ret[2]);
  }
  {
    auto ret = rcpputils::split("hello\\world\\", '\\', true);
    EXPECT_EQ(2u, ret.size());
    EXPECT_EQ("hello", ret[0]);
    EXPECT_EQ("world", ret[1]);
  }
  {
    auto ret = rcpputils::split("hello\\world\\", '\\', false);
    EXPECT_EQ(2u, ret.size());
    EXPECT_EQ("hello", ret[0]);
    EXPECT_EQ("world", ret[1]);
  }
  {
    auto ret = rcpputils::split("hello\\\\world", '\\', true);
    EXPECT_EQ(2u, ret.size());
    EXPECT_EQ("hello", ret[0]);
    EXPECT_EQ("world", ret[1]);
  }
  {
    auto ret = rcpputils::split("hello\\\\world", '\\', false);
    EXPECT_EQ(3u, ret.size());
    EXPECT_EQ("hello", ret[0]);
    EXPECT_EQ("", ret[1]);
    EXPECT_EQ("world", ret[2]);
  }
  {
    auto ret = rcpputils::split("\\my\\hello\\world", '\\', true);
    EXPECT_EQ(3u, ret.size());
    EXPECT_EQ("my", ret[0]);
    EXPECT_EQ("hello", ret[1]);
    EXPECT_EQ("world", ret[2]);
  }
  {
    auto ret = rcpputils::split("\\my\\hello\\world", '\\', false);
    EXPECT_EQ(4u, ret.size());
    EXPECT_EQ("", ret[0]);
    EXPECT_EQ("my", ret[1]);
    EXPECT_EQ("hello", ret[2]);
    EXPECT_EQ("world", ret[3]);
  }
  {
    auto ret = rcpputils::split("\\my\\\\hello\\\\world\\", '\\', true);
    EXPECT_EQ(3u, ret.size());
    EXPECT_EQ("my", ret[0]);
    EXPECT_EQ("hello", ret[1]);
    EXPECT_EQ("world", ret[2]);
  }
  {
    auto ret = rcpputils::split("\\my\\\\hello\\\\world\\", '\\', false);
    EXPECT_EQ(6u, ret.size());
    EXPECT_EQ("", ret[0]);
    EXPECT_EQ("my", ret[1]);
    EXPECT_EQ("", ret[2]);
    EXPECT_EQ("hello", ret[3]);
    EXPECT_EQ("", ret[4]);
    EXPECT_EQ("world", ret[5]);
  }
}
