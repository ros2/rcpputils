// Copyright (c) 2019, Open Source Robotics Foundation, Inc.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//
//    * Neither the name of the copyright holder nor the names of its
//      contributors may be used to endorse or promote products derived from
//      this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

// This file is originally from:
// https://github.com/ros/pluginlib/blob/1a4de29fa55173e9b897ca8ff57ebc88c047e0b3/pluginlib/include/pluginlib/impl/filesystem_helper.hpp

/*! \file filesystem_helper.hpp
 * \brief Cross-platform filesystem helper functions and additional emulation of [std::filesystem](https://en.cppreference.com/w/cpp/filesystem).
 *
 * Note: Once std::filesystem is supported on all ROS2 platforms, this class
 * can be deprecated/removed in favor of the built-in functionality.
 */

#include "rcpputils/filesystem_helper.hpp"

#include <sys/stat.h>

#include <algorithm>
#include <cstdint>
#include <climits>
#include <cstring>
#include <random>
#include <string>
#include <system_error>
#include <stdexcept>
#include <vector>

#ifdef _WIN32
#  define NOMINMAX
#  define NOGDI
#  include <windows.h>
#  include <direct.h>
#  include <fileapi.h>
#  include <io.h>
#  define access _access_s
#else
#  include <dirent.h>
#  include <sys/types.h>
#  include <unistd.h>
#endif

#include "rcutils/env.h"
#include "rcpputils/scope_exit.hpp"
#include "rcpputils/split.hpp"

namespace rcpputils
{
namespace fs
{
std::filesystem::path create_temporary_directory(
  const std::string & base_name, const std::filesystem::path & parent_path, size_t max_tries)
{
  if (base_name.find(std::filesystem::path::preferred_separator) != std::string::npos) {
    throw std::invalid_argument("The base_name contain directory-separator");
  }
  // mersenne twister random generator engine seeded with the std::random_device
  std::mt19937 random_generator(std::random_device{}());
  std::uniform_int_distribution<> distribution(0, 0xFFFFFF);
  std::filesystem::path path_to_temp_dir;
  constexpr size_t kSuffixLength = 7;  // 6 chars + 1 null terminator
  char random_suffix_str[kSuffixLength];
  size_t current_iteration = 0;
  while (true) {
    snprintf(random_suffix_str, kSuffixLength, "%06x", distribution(random_generator));
    const std::string random_dir_name = base_name + random_suffix_str;
    path_to_temp_dir = parent_path / random_dir_name;
    // true if the directory was newly created.
    if (std::filesystem::create_directories(path_to_temp_dir)) {
      break;
    }
    if (current_iteration == max_tries) {
      throw std::runtime_error(
        "Exceeded maximum allowed iterations to find non-existing directory");
    }
    current_iteration++;
  }
  return path_to_temp_dir;
}
}  // namespace fs
}  // namespace rcpputils
