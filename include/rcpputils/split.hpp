// Copyright (c) 2017, Open Source Robotics Foundation, Inc.
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
// https://github.com/ros/pluginlib/blob/1a4de29fa55173e9b897ca8ff57ebc88c047e0b3/pluginlib/include/pluginlib/impl/split.hpp

#ifndef RCPPUTILS__SPLIT_HPP_
#define RCPPUTILS__SPLIT_HPP_

#include <algorithm>
#include <regex>
#include <string>
#include <vector>

namespace rcpputils
{

/// Split a specified input with a regular expression
/**
 * The returned vector will contain the tokens split from the input
 *
 * \param[in] input the input string to be split
 * \param[in] regex the regular expression to match to delimit tokens
 * \return A vector of tokens.
 */
inline std::vector<std::string>
split(const std::string & input, const std::string & regex, bool skip_empty = false)
{
  std::regex re(regex);
  // the -1 will cause this to return the stuff between the matches, see the submatch argument:
  //   http://en.cppreference.com/w/cpp/regex/regex_token_iterator/regex_token_iterator
  std::sregex_token_iterator first(input.begin(), input.end(), re, -1);
  std::sregex_token_iterator last;

  std::vector<std::string> ret;
  std::copy_if(first, last, std::back_inserter(ret),
    [skip_empty](std::string val) {
      return !(skip_empty && val.empty());
    });
  return ret;
}
}  // namespace rcpputils

#endif  // RCPPUTILS__SPLIT_HPP_
