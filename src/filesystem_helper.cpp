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

#include <cstring>
#include <string>
#include <system_error>

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

#include "rcutils/get_env.h"

namespace rcpputils
{
namespace fs
{

bool path::exists() const
{
  return access(path_.c_str(), 0) == 0;
}

bool path::is_directory() const noexcept
{
  struct stat stat_buffer;
  const auto rc = stat(path_.c_str(), &stat_buffer);

  if (rc != 0) {
    return false;
  }

#ifdef _WIN32
  return (stat_buffer.st_mode & S_IFDIR) == S_IFDIR;
#else
  return S_ISDIR(stat_buffer.st_mode);
#endif
}

bool path::is_regular_file() const noexcept
{
  struct stat stat_buffer;
  const auto rc = stat(path_.c_str(), &stat_buffer);

  if (rc != 0) {
    return false;
  }

#ifdef _WIN32
  return (stat_buffer.st_mode & S_IFREG) == S_IFREG;
#else
  return S_ISREG(stat_buffer.st_mode);
#endif
}

path temp_directory_path()
{
#ifdef _WIN32
#ifdef UNICODE
#error "rcpputils::fs does not support Unicode paths"
#endif
  TCHAR temp_path[MAX_PATH];
  DWORD size = GetTempPathA(MAX_PATH, temp_path);
  if (size > MAX_PATH || size == 0) {
    std::error_code ec(static_cast<int>(GetLastError()), std::system_category());
    throw std::system_error(ec, "cannot get temporary directory path");
  }
  temp_path[size] = '\0';
#else
  const char * temp_path = NULL;
  const char * ret_str = rcutils_get_env("TMPDIR", &temp_path);
  if (NULL != ret_str || *temp_path == '\0') {
    temp_path = "/tmp";
  }
#endif
  return path(temp_path);
}

path current_path()
{
#ifdef _WIN32
#ifdef UNICODE
#error "rcpputils::fs does not support Unicode paths"
#endif
  char cwd[MAX_PATH];
  if (nullptr == _getcwd(cwd, MAX_PATH)) {
#else
  char cwd[PATH_MAX];
  if (nullptr == getcwd(cwd, PATH_MAX)) {
#endif
    std::error_code ec{errno, std::system_category()};
    errno = 0;
    throw std::system_error{ec, "cannot get current working directory"};
  }

  return path(cwd);
}

bool create_directories(const path & p)
{
  path p_built;
  int status = 0;

  for (auto it = p.cbegin(); it != p.cend() && status == 0; ++it) {
    if (!p_built.empty() || it->empty()) {
      p_built /= *it;
    } else {
      p_built = *it;
    }
    if (!p_built.exists()) {
#ifdef _WIN32
      status = _mkdir(p_built.string().c_str());
#else
      status = mkdir(p_built.string().c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
#endif
    }
  }
  return status == 0 && p_built.is_directory();
}

bool remove(const path & p)
{
#ifdef _WIN32
  struct _stat s;
  if (_stat(p.string().c_str(), &s) == 0) {
    if (s.st_mode & S_IFDIR) {
      return _rmdir(p.string().c_str()) == 0;
    }
    if (s.st_mode & S_IFREG) {
      return ::remove(p.string().c_str()) == 0;
    }
  }
  return false;
#else
  return ::remove(p.string().c_str()) == 0;
#endif
}

bool remove_all(const path & p)
{
  if (!is_directory(p)) {return rcpputils::fs::remove(p);}

#ifdef _WIN32
  // We need a string of type PCZZTSTR, which is a double null terminated char ptr
  size_t length = p.string().size();
  TCHAR * temp_dir = new TCHAR[length + 2];
  memcpy(temp_dir, p.string().c_str(), length);
  temp_dir[length] = '\0';
  temp_dir[length + 1] = '\0';  // double null terminated

  SHFILEOPSTRUCT file_options;
  file_options.hwnd = nullptr;
  file_options.wFunc = FO_DELETE;  // delete (recursively)
  file_options.pFrom = temp_dir;
  file_options.pTo = nullptr;
  file_options.fFlags = FOF_NOCONFIRMATION | FOF_SILENT;  // do not prompt user
  file_options.fAnyOperationsAborted = FALSE;
  file_options.lpszProgressTitle = nullptr;
  file_options.hNameMappings = nullptr;

  auto ret = SHFileOperation(&file_options);
  delete[] temp_dir;

  return 0 == ret && false == file_options.fAnyOperationsAborted;
#else
  DIR * dir = opendir(p.string().c_str());
  struct dirent * directory_entry;
  while ((directory_entry = readdir(dir)) != nullptr) {
    // Make sure to not call ".." or "." entries in directory (might delete everything)
    if (strcmp(directory_entry->d_name, ".") != 0 && strcmp(directory_entry->d_name, "..") != 0) {
      auto sub_path = rcpputils::fs::path(p) / directory_entry->d_name;
      // if directory, call recursively
      if (sub_path.is_directory() && !rcpputils::fs::remove_all(sub_path)) {
        return false;
        // if not, call regular remove
      } else if (!rcpputils::fs::remove(sub_path)) {
        return false;
      }
    }
  }
  closedir(dir);
  // directory is empty now, call remove
  rcpputils::fs::remove(p);
  return !rcpputils::fs::exists(p);
#endif
}

}  // namespace fs
}  // namespace rcpputils
