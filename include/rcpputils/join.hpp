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

#ifndef RCPPUTILS__JOIN_HPP_
#define RCPPUTILS__JOIN_HPP_

#include <algorithm>
#include <iterator>
#include <sstream>
#include <string>

namespace rcpputils
{

/// Join values in a container turned into strings by a given delimiter
/**
 * \param[in] container is a collection of values to be turned into string and joined.
 * \param[in] delim is a delimiter to join values turned into strings.
 * \tparam CharT is the string character type.
 * \tparam ValueT is the container value type.
 * \tparam AllocatorT is the container allocator type.
 * \tparam ContainerT is the container template type.
 * \return joined string
 */
template<
  typename CharT, typename ValueT, typename AllocatorT,
  template<typename T, class A> class ContainerT>
std::basic_string<CharT>
join(const ContainerT<ValueT, AllocatorT> & container, const CharT * delim)
{
  std::basic_ostringstream<CharT> s;
  std::copy(container.begin(), container.end(), std::ostream_iterator<ValueT, CharT>(s, delim));
  std::basic_string<CharT> result = s.str();
  if (delim) {
    result = result.substr(0, result.length() - strlen(delim));
  }
  return result;
}

}  // namespace rcpputils

#endif  // RCPPUTILS__JOIN_HPP_
