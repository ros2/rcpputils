// Copyright (c) 2019, Open Source Robotics Foundation, Inc.
// All rights reserved.
//
// Software License Agreement (BSD License 2.0)
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above
//    copyright notice, this list of conditions and the following
//    disclaimer in the documentation and/or other materials provided
//    with the distribution.
//  * Neither the name of the copyright holders nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
// ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// This file is originally from:
// https://github.com/ros/pluginlib/blob/1a4de29fa55173e9b897ca8ff57ebc88c047e0b3/pluginlib/include/pluginlib/impl/filesystem_helper.hpp

/**
 * If std::filesystem is not available the necessary functions are emulated.
 *
 * Note: Once std::filesystem is supported on all ROS2 platforms, this class
 * can be deprecated in favor of the built-in functionality.
 */

#ifndef RCPPUTILS__FILESYSTEM_HELPER_HPP_
#define RCPPUTILS__FILESYSTEM_HELPER_HPP_

#include <algorithm>
#include <string>
#include <vector>

#ifdef _WIN32
#  define RCPPUTILS_IMPL_OS_DIRSEP '\\'
#else
#  define RCPPUTILS_IMPL_OS_DIRSEP '/'
#endif

#ifdef _WIN32
#  include <io.h>
#  define access _access_s
#else
#  include <unistd.h>
#endif

#include "rcpputils/split.hpp"

namespace rcpputils
{
namespace fs
{

static constexpr const char kPreferredSeparator = RCPPUTILS_IMPL_OS_DIRSEP;

class path
{
public:
  path()
  : path("")
  {}

  path(const std::string & p)  // NOLINT(runtime/explicit): this is a conversion constructor
  : path_(p), path_as_vector_(split(p, kPreferredSeparator))
  {
    std::replace(path_.begin(), path_.end(), '\\', kPreferredSeparator);
    std::replace(path_.begin(), path_.end(), '/', kPreferredSeparator);
  }

  std::string string() const
  {
    return path_;
  }

  bool exists() const
  {
    return access(path_.c_str(), 0) == 0;
  }

  bool is_absolute() const
  {
    return path_.compare(0, 1, "/") == 0 || path_.compare(1, 2, ":\\") == 0;
  }

  std::vector<std::string>::const_iterator cbegin() const
  {
    return path_as_vector_.cbegin();
  }

  std::vector<std::string>::const_iterator cend() const
  {
    return path_as_vector_.cend();
  }

  path parent_path() const
  {
    path parent("");
    for (auto it = this->cbegin(); it != --this->cend(); ++it) {
      parent /= *it;
    }
    return parent;
  }

  path filename() const
  {
    return path_.empty() ? path() : *--this->cend();
  }

  path operator/(const std::string & other)
  {
    return this->operator/(path(other));
  }

  path & operator/=(const std::string & other)
  {
    this->operator/=(path(other));
    return *this;
  }

  path operator/(const path & other)
  {
    return path(*this).operator/=(other);
  }

  path & operator/=(const path & other)
  {
    this->path_ += kPreferredSeparator + other.string();
    this->path_as_vector_.insert(
      std::end(this->path_as_vector_),
      std::begin(other.path_as_vector_), std::end(other.path_as_vector_));
    return *this;
  }

private:
  std::string path_;
  std::vector<std::string> path_as_vector_;
};

inline bool exists(const path & path_to_check)
{
  return path_to_check.exists();
}

#undef RCPPUTILS_IMPL_OS_DIRSEP

}  // namespace fs
}  // namespace rcpputils

#endif  // RCPPUTILS__FILESYSTEM_HELPER_HPP_
