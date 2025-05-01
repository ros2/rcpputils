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
#include "rcpputils/env.hpp"

#ifdef _WIN32
static constexpr const bool is_win32 = true;
#else
static constexpr const bool is_win32 = false;
#endif

TEST(TestFilesystemHelper, create_temporary_directory)
{
  // basic usage
  {
    const std::string basename = "test_base_name";

    const auto tmpdir1_std = rcpputils::fs::create_temporary_directory(basename);
    std::filesystem::path tmpdir1(tmpdir1_std.generic_string());
    EXPECT_TRUE(std::filesystem::exists(tmpdir1));
    EXPECT_TRUE(std::filesystem::is_directory(tmpdir1));

    auto tmp_file = tmpdir1 / "test_file.txt";
    {
      std::ofstream output_buffer{tmp_file.string()};
      output_buffer << "test";
    }
    EXPECT_TRUE(std::filesystem::exists(tmp_file));
    EXPECT_TRUE(std::filesystem::is_regular_file(tmp_file));

    const auto tmpdir2_std = rcpputils::fs::create_temporary_directory(basename);
    std::filesystem::path tmpdir2(tmpdir2_std.generic_string());
    EXPECT_TRUE(std::filesystem::exists(tmpdir2));
    EXPECT_TRUE(std::filesystem::is_directory(tmpdir2));

    EXPECT_NE(tmpdir1.string(), tmpdir2.string());

    EXPECT_TRUE(std::filesystem::remove_all(tmpdir1));
    EXPECT_TRUE(std::filesystem::remove_all(tmpdir2));
  }

  // bad names
  {
    if (is_win32) {
      EXPECT_THROW(rcpputils::fs::create_temporary_directory("illegalchar?"), std::system_error);
      EXPECT_THROW(rcpputils::fs::create_temporary_directory("base\\name"), std::invalid_argument);
    } else {
      EXPECT_THROW(rcpputils::fs::create_temporary_directory("base/name"), std::invalid_argument);
    }
  }

  // newly created paths
  {
    const auto new_relative = std::filesystem::current_path() / "child1" / "child2";
    const auto tmpdir_std = rcpputils::fs::create_temporary_directory(
      "base_name",
      std::filesystem::path(new_relative.string()));
    std::filesystem::path tmpdir(tmpdir_std.generic_string());

    EXPECT_TRUE(std::filesystem::exists(tmpdir));
    EXPECT_TRUE(std::filesystem::is_directory(tmpdir));
    EXPECT_TRUE(std::filesystem::remove_all(tmpdir));
  }

  // edge case inputs
  {
    // Provided no base name we should still get a path with the 6 unique template chars
    const auto tmpdir_emptybase = rcpputils::fs::create_temporary_directory("");
    EXPECT_EQ(tmpdir_emptybase.filename().string().size(), 6u);

    // With the template string XXXXXX already in the name, it will still be there, the unique
    // portion is appended to the end.
    const auto tmpdir_template_in_name_std =
      rcpputils::fs::create_temporary_directory("base_XXXXXX");
    std::filesystem::path tmpdir_template_in_name(tmpdir_template_in_name_std.generic_string());

    EXPECT_TRUE(std::filesystem::exists(tmpdir_template_in_name));
    EXPECT_TRUE(std::filesystem::is_directory(tmpdir_template_in_name));
    // On Linux, it will not replace the base_name Xs, only the final 6 that the function appends.
    // On OSX, it will replace _all_ trailing Xs.
    // Either way, the result is unique, the exact value doesn't matter.
    EXPECT_EQ(tmpdir_template_in_name.filename().string().rfind("base_", 0), 0u);
  }
}
