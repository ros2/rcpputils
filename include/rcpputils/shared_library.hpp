// Copyright 2020 Open Source Robotics Foundation, Inc.
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

#ifndef RCPPUTILS__SHARED_LIBRARY_HPP_
#define RCPPUTILS__SHARED_LIBRARY_HPP_

#include <string>
#include <stdexcept>

#include "rcutils/shared_library.h"

namespace rcpputils
{

/**
 * This class is an abstraction of rcutils shared library to be able to used it
 *  with modern C++
 **/
class SharedLibrary
{
public:
  /// The library is loaded in the constructor
  /**
   * \param[in] input The library string path.
   */
  explicit SharedLibrary(std::string library_path);

  /// The library is unloaded in the deconstructor
  ~SharedLibrary();

  /// Return true if the shared library contains a specific symbol name otherwise returns false.
  /**
   * \param[in] symbol_name name of the symbol inside the shared library
   * \return if symbols exists returns true, otherwise returns false.
   */
  bool has_symbol(std::string symbol_name);

  /// Return shared library symbol pointer.
  /**
   * \param[in] symbol_name name of the symbol inside the shared library
   * \return shared library symbol pointer, if the symbol doesn't exist then returns NULL.
   */
  void * get_symbol(std::string symbol_name);

private:
  rcutils_shared_library_t lib;
};

}  // namespace rcpputils

#endif  // RCPPUTILS__SHARED_LIBRARY_HPP_
