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

#include "rcpputils/find_library.hpp"

#include <cassert>
#include <cstddef>

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "rcutils/filesystem.h"

#include "rcpputils/filesystem_helper.hpp"
#include "rcpputils/split.hpp"
#include "rcpputils/env.hpp"

namespace rcpputils
{

namespace
{

#ifdef _WIN32
static constexpr char kPathVar[] = "PATH";
static constexpr char kPathSeparator = ';';
static constexpr char kSolibPrefix[] = "";
static constexpr char kSolibExtension[] = ".dll";
#elif __APPLE__
static constexpr char kPathVar[] = "DYLD_LIBRARY_PATH";
static constexpr char kPathSeparator = ':';
static constexpr char kSolibPrefix[] = "lib";
static constexpr char kSolibExtension[] = ".dylib";
#else
static constexpr char kPathVar[] = "LD_LIBRARY_PATH";
static constexpr char kPathSeparator = ':';
static constexpr char kSolibPrefix[] = "lib";
static constexpr char kSolibExtension[] = ".so";
#endif

}  // namespace

std::string find_library_path(const std::string & library_name)
{
  std::string search_path = get_env_var(kPathVar);
  std::vector<std::string> search_paths = rcpputils::split(search_path, kPathSeparator);

  std::string filename = filename_for_library(library_name);

  for (const auto & search_path : search_paths) {
    std::string path = search_path + "/" + filename;
    if (rcutils_is_file(path.c_str())) {
      return path;
    }
  }
  return "";
}

std::string path_for_library(const std::string & directory, const std::string & library_name)
{
  auto path = std::filesystem::path(directory) / filename_for_library(library_name);
  if (std::filesystem::is_regular_file(path)) {
    return path.string();
  }
  return "";
}

std::string filename_for_library(const std::string & library_name)
{
  return kSolibPrefix + library_name + kSolibExtension;
}

}  // namespace rcpputils
