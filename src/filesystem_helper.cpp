// Copyright 2019 Amazon.com, Inc. or its affiliates. All Rights Reserved.
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

#include <sys/stat.h>

#include <string>

#include "rcpputils/filesystem_helper.hpp"

std::uintmax_t rcpputils::fs::file_size(const path & file_path)
{
  if (!file_path.exists()) {
    std::error_code ec{ENOENT, std::system_category()};
    const std::string error_msg{"Path " + file_path.string() + " does not exist"};
    throw std::system_error(ec, error_msg);
  }
  struct stat stat_buffer {};
  const int rc = stat(file_path.string().c_str(), &stat_buffer);
  return rc == 0 ? static_cast<size_t>(stat_buffer.st_size) : 0;
}
