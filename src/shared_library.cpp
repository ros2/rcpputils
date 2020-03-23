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

#include <iostream>
#include <string>

#include "rcpputils/shared_library.hpp"

namespace rcpputils
{
SharedLibrary::SharedLibrary(const std::string & library_path)
{
  lib = rcutils_get_zero_initialized_shared_library();
  rcutils_ret_t ret = rcutils_load_shared_library(
    &lib,
    library_path.c_str(),
    rcutils_get_default_allocator());
  if (ret != RCUTILS_RET_OK) {
    if (ret == RCUTILS_RET_BAD_ALLOC) {
      throw std::runtime_error{"rcutils shared_library exception: failed to allocate memory"};
    } else if (ret == RCUTILS_RET_INVALID_ARGUMENT) {
      throw std::runtime_error{"rcutils shared_library exception: invalid arguments"};
    } else {
      throw std::runtime_error{"rcutils shared_library exception: library could not be found:" +
              library_path};
    }
  }
}

SharedLibrary::~SharedLibrary()
{
  rcutils_ret_t ret = rcutils_unload_shared_library(&lib);
  if (ret != RCUTILS_RET_OK) {
    std::cerr << "rcutils shared_library exception: failed destroying object" << std::endl;
  }
}

void * SharedLibrary::get_symbol(const std::string & symbol_name)
{
  void * lib_symbol = rcutils_get_symbol(&lib, symbol_name.c_str());

  if (!lib_symbol) {
    throw std::runtime_error{"rcpputils SharedLiibrary exception: symbol_name could not be found:" +
            symbol_name + " in " + lib.library_path};
  }
  return lib_symbol;
}

bool SharedLibrary::has_symbol(const std::string & symbol_name)
{
  return rcutils_has_symbol(&lib, symbol_name.c_str());
}

std::string SharedLibrary::get_library_path()
{
  if (lib.library_path != nullptr) {
    return std::string(lib.library_path);
  }
  return nullptr;
}
}  // namespace rcpputils
