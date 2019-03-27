cmake_minimum_required(VERSION 3.5)

project(rcpputils)

find_package(ament_cmake REQUIRED)

# Default to C11
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 11)
endif()
# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(NOT WIN32)
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

include_directories(include)

ament_export_include_directories(include)

if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)
  find_package(ament_lint_auto REQUIRED)

  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-Wthread-safety -Werror)
  endif()

  ament_lint_auto_find_test_dependencies()

  ament_add_gtest(test_basic test/test_basic.cpp)

  ament_add_gtest(test_filesystem_helper test/test_filesystem_helper.cpp)

  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND CMAKE_CXX_FLAGS MATCHES "-stdlib=libc\\+\\+")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 7.0)
      set(FILESYSTEM_LIB c++experimental)
    else()
      set(FILESYSTEM_LIB c++fs)
    endif()
  else()
    set(FILESYSTEM_LIB stdc++fs)
  endif()

  target_link_libraries(test_filesystem_helper ${FILESYSTEM_LIB})

endif()

ament_package()

install(
  DIRECTORY include/
  DESTINATION include)
