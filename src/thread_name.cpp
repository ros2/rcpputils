// Copyright 2025 Open Source Robotics Foundation, Inc.
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

#include "rcpputils/thread_name.hpp"

#if defined(_WIN32)
#include <windows.h>
#include <vector>
#else  // posix and apple
#include <pthread.h>
#endif  // defined(_WIN32)

#include <cstddef>
#include <string>
#include <system_error>

namespace rcpputils
{

#if defined(_WIN32)
void set_thread_name_windows(const std::string & name)
{
  int wchars_num = MultiByteToWideChar(CP_UTF8, 0, name.c_str(), -1, nullptr, 0);
  bool conversion_failed = wchars_num == 0;
  if (!conversion_failed) {
    std::vector<wchar_t> wstr(wchars_num);
    auto ret = MultiByteToWideChar(CP_UTF8, 0, name.c_str(), -1, wstr.data(), wchars_num);
    bool conversion_failed = ret == 0;
    if (!conversion_failed) {
          // This will only work on Windows 10 and above.
      auto result = SetThreadDescription(GetCurrentThread(), wstr.data());
      if (FAILED(result)) {
        std::error_code error_code(result, std::system_category());
        throw std::system_error(error_code, "Failed to set thread description");
      }
    }
  }
  if (conversion_failed) {
    std::error_code error_code_obj(GetLastError(), std::system_category());
    throw std::system_error(error_code_obj, "Failed to convert thread name to wide characters");
  }
}
#else

// This includes the null terminator
#if defined(__APPLE__)
constexpr std::size_t MAXTHREADNAMESIZE = 64;
#else  // posix
constexpr std::size_t MAXTHREADNAMESIZE = 16;
#endif  // defined(__APPLE__)

void set_thread_name_posix(const std::string & name)
{
  std::string truncated_name;  // At this scope to keep lifetime long enough
  const char * thread_name_ptr;
  // Truncate name to maximum length supported by pthread_setname_np
  // leaving one character for the null terminator
  // otherwise pthread_setname_np will return an ERANGE error
  if (name.size() > MAXTHREADNAMESIZE - 1) {
    truncated_name = name.substr(0, MAXTHREADNAMESIZE - 1);
    thread_name_ptr = truncated_name.c_str();
  } else {
    thread_name_ptr = name.c_str();
  }
#if defined(__APPLE__)
  int rc = pthread_setname_np(thread_name_ptr);
#else  // posix
  int rc = pthread_setname_np(pthread_self(), thread_name_ptr);
#endif  // defined(__APPLE__)
  if (rc != 0) {
    std::error_code error_code(rc, std::system_category());
    throw std::system_error(error_code, "Failed to set thread name");
  }
}

#endif  // defined(_WIN32)

void set_thread_name(const std::string & name)
{
#if defined(_WIN32)
  set_thread_name_windows(name);
#else
  set_thread_name_posix(name);
#endif  // defined(_WIN32)
}

std::string get_thread_name()
{
  std::string name;
#if defined(_WIN32)
  // This will only work on Windows 10 and above.
  PWSTR thread_description;
  auto result = GetThreadDescription(GetCurrentThread(), &thread_description);
  if (FAILED(result)) {
    std::error_code error_code(result, std::system_category());
    throw std::system_error(error_code, "Failed to get thread name");
  }
  int size_needed = WideCharToMultiByte(CP_UTF8, 0, thread_description, -1, nullptr, 0, nullptr,
      nullptr);
  bool conversion_failed = size_needed == 0;
  if (!conversion_failed) {
    name = std::string(size_needed - 1, 0);
    auto ret = WideCharToMultiByte(CP_UTF8, 0, thread_description, -1, &name[0], size_needed,
        nullptr, nullptr);
    conversion_failed = ret == 0;
  }
  LocalFree(thread_description);
  if (conversion_failed) {
    DWORD error_code = GetLastError();
    std::error_code error_code_obj(error_code, std::system_category());
    throw std::system_error(error_code_obj, "Failed to convert thread name from wide characters");
  }
#else  // posix and apple
  char thread_name[MAXTHREADNAMESIZE];  // This includes the null terminator
  int rc = pthread_getname_np(pthread_self(), thread_name, sizeof(thread_name));
  if (rc != 0) {
    std::error_code error_code(rc, std::system_category());
    throw std::system_error(error_code, "Failed to get thread name");
  }
  name = std::string(thread_name);
#endif
  return name;
}

}  // namespace rcpputils
