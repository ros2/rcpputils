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

#include "gtest/gtest.h"

#include "rcpputils/endian.hpp"

// Basic runtime endianness check
bool isLittleEndian()
{
  uint16_t number = 0x0001;
  uint8_t * numPtr = reinterpret_cast<uint8_t *>(&number);
  return numPtr[0] == 1;
}

TEST(test_endian, is_defined)
{
  // A platform should be one or other other
  EXPECT_TRUE((rcpputils::endian::little == rcpputils::endian::native) ||
    (rcpputils::endian::big == rcpputils::endian::native));
}

TEST(test_endian, runtime_endianness)
{
  // Compare the runtime evaluation to the predefined constant value.
  EXPECT_TRUE(isLittleEndian() &&
    rcpputils::endian::little == rcpputils::endian::native);
}

#ifdef _WIN32
// Windows is always little-endian
TEST(test_endian, win32)
{
  EXPECT_TRUE(rcpputils::endian::little == rcpputils::endian::native);
}
#endif
