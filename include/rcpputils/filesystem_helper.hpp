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

#ifndef RCPPUTILS__FILESYSTEM_HELPER_HPP_
#define RCPPUTILS__FILESYSTEM_HELPER_HPP_

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

#include "rcpputils/visibility_control.hpp"

namespace rcpputils
{
namespace fs
{

#ifdef _WIN32
#  define RCPPUTILS_IMPL_OS_DIRSEP '\\'
#else
#  define RCPPUTILS_IMPL_OS_DIRSEP '/'
#endif

/**
 * \def kPreferredSeparator
 *
 * A definition for this platforms string path separator
 */
static constexpr const char kPreferredSeparator = RCPPUTILS_IMPL_OS_DIRSEP;

#undef RCPPUTILS_IMPL_OS_DIRSEP

/// \brief Construct a uniquely named temporary directory, in "parent", with format base_nameXXXXXX
/// The output, if successful, is guaranteed to be a newly-created directory.
/// The underlying implementation keeps generating paths until one that does not exist is found or
/// until the number of iterations exceeded the maximum tries.
/// This guarantees that there will be no existing files in the returned directory.
/// \param[in] base_name User-specified portion of the created directory.
/// \param[in] parent_path The parent path of the directory that will be created.
/// \param[in] max_tries The maximum number of tries to find a unique directory (default 1000)
/// \return A path to a newly created directory with base_name and a 6-character unique suffix.
/// \throws std::invalid_argument If base_name contain directory-separator defined as
/// std::filesystem::path::preferred_separator.
/// \throws std::system_error If any OS APIs do not succeed.
/// \throws std::runtime_error If the number of the iterations exceeds the maximum tries and
/// a unique directory is not found.
RCPPUTILS_PUBLIC std::filesystem::path create_temporary_directory(
  const std::string & base_name,
  const std::filesystem::path & parent_path = std::filesystem::temp_directory_path(),
  size_t max_tries = 1000);
}  // namespace fs
}  // namespace rcpputils

#endif  // RCPPUTILS__FILESYSTEM_HELPER_HPP_
