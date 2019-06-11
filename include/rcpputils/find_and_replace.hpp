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

#ifndef RCPPUTILS__FIND_AND_REPLACE_HPP_
#define RCPPUTILS__FIND_AND_REPLACE_HPP_

#include <string>

namespace rcpputils
{

/// Find and replace all instances of a string with another string.
/**
 * \param[in] input The input string.
 * \param[in] find The substring to replace.
 * \param[in] replace The string to substitute for each occurrence of `find`.
 * \return A copy of the input string with all instances of the string `find` replaced with the
 *   string `replace`.
 */
inline std::string
find_and_replace(const std::string & input, const std::string & find, const std::string & replace)
{
  std::string output = input;
  const std::size_t find_len = find.length();
  const std::size_t replace_len = replace.length();
  if (0u == find_len) {
    return output;
  }
  std::size_t pos = output.find(find);
  while (pos != std::string::npos) {
    output.replace(pos, find_len, replace);
    pos = output.find(find, pos + replace_len);
  }
  return output;
}

}  // namespace rcpputils

#endif  // RCPPUTILS__FIND_AND_REPLACE_HPP_
