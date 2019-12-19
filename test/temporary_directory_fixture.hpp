// Copyright 2018, Bosch Software Innovations GmbH.
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

#ifndef TEMPORARY_DIRECTORY_FIXTURE_HPP_
#define TEMPORARY_DIRECTORY_FIXTURE_HPP_

#include <gmock/gmock.h>

#include <string>

#ifdef _WIN32
# include <direct.h>
# include <Windows.h>
#include <Shellapi.h>
#include <shlobj.h>
#else
# include <unistd.h>
# include <sys/types.h>
# include <dirent.h>
#endif

namespace
{
#ifdef _WIN32
/**
 * Create a string of type PCZZTSTR, which is a double null terminated char pointer.
 *
 * \param text The string to convert.
 */
std::unique_ptr<TCHAR> create_double_null_terminated_tchar(const std::string & text)
{
  // Increase length by two to add the double null termination
  const auto char_length = strlen(text.c_str());
  const auto new_text = std::make_unique<TCHAR[]>(char_length + 2);
  // memcpy(temp_dir, text.c_str(), length);
  _tcscpy(new_text, text.c_str())
  // Double null terminate
  new_text[length] = L'\0';
  new_text[length + 1] = L'\0';
  return new_text;
}
#else
void check_directory(DIR * directory)
{
  if (!directory) {
    return;
  }
}

bool directory_entry_is_clean(const struct dirent * directory_entry)
{
  return strcmp(directory_entry->d_name, ".") != 0 && strcmp(directory_entry->d_name, "..") != 0;
}

bool entry_is_directory(struct dirent * directory_entry)
{
  return directory_entry->d_type == DT_DIR;
}
#endif

}  // namespace

class TemporaryDirectoryFixture : public ::testing::Test
{
public:
  TemporaryDirectoryFixture() = default;

  void SetUp() override
  {
    char template_char[] = "tmp_test_dir.XXXXXX";
#ifdef _WIN32
    char temp_path[255];
    GetTempPathA(255, temp_path);
    _mktemp_s(template_char, strnlen(template_char, 20) + 1);
    temporary_dir_path_ = std::string(temp_path) + std::string(template_char);
    _mkdir(temporary_dir_path_.c_str());
#else
    char * dir_name = mkdtemp(template_char);
    temporary_dir_path_ = dir_name;
#endif
  }

  void TearDown() override
  {
    remove_directory_recursively(temporary_dir_path_);
  }

  void remove_directory_recursively(const std::string & directory_path)
  {
#ifdef _WIN32
    const auto null_terminated_directory = create_double_null_terminated_tchar(directory_path);

    SHFILEOPSTRUCT file_options{
      .hwnd = nullptr;
      .wFunc = FO_DELETE;  // Delete (recursively)
      .pFrom = temp_dir;
      .pTo = nullptr;
      .fFlags = FOF_NOCONFIRMATION | FOF_SILENT;  // Do not prompt user
      .fAnyOperationsAborted = FALSE;
      .lpszProgressTitle = nullptr;
      .hNameMappings = nullptr;
    };

    const auto operation_result = SHFileOperation(&file_options);
    if (operation_result) {
      throw std::runtime_error("Unable to delete directory. You have a leaking directory");
    }
#else
    DIR * directory = opendir(directory_path.c_str());
    check_directory(directory);  // Directory has no entries, exit.
    auto directory_entry = readdir(directory);

    // Iterate through all elements in the directory.
    // If the element is a directory, remove all sub-elements recursively.
    try{
      do {
        if (directory_entry_is_clean(directory_entry)) {
          const auto full_file_path{directory_path + "/" + directory_entry->d_name};
          if (entry_is_directory(directory_entry)) {
            remove_directory_recursively(full_file_path);
          }
          remove(full_file_path.c_str());
        }
        directory_entry = readdir(directory);
      } while (directory_entry != nullptr);
    } catch (...) {
      closedir(directory);
      throw std::runtime_error("Unable to close directory. You have a leaking directory.");
    }
    closedir(directory);

    // Remove the top level (now empty) directory
    remove(temporary_dir_path_.c_str());
#endif
  }

  std::string temporary_dir_path_;
};

#endif  // TEMPORARY_DIRECTORY_FIXTURE_HPP_
