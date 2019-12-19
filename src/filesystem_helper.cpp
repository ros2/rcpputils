
#include "rcpputils/filesystem_helper.hpp"

std::uintmax_t rcpputils::fs::file_size(const path & file_path)
{
  if (!file_path.exists()) {
    std::error_code ec{ENOENT, std::system_category()};
    const std::string error_msg{"Path " + file_path.string() + " does not exist"};
    throw std::system_error(ec, error_msg);
  }
  struct stat stat_buffer{};
  const int rc = stat(file_path.string().c_str(), &stat_buffer);
  return rc == 0 ? static_cast<size_t>(stat_buffer.st_size) : 0;
}
