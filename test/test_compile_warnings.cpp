// Copyright 2025 cellumation GmbH
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

#include <rcpputils/compile_warnings.hpp>
struct CompileTest
{
  [[deprecated]]
  void deprecated_function()
  {
  }
};

void compile_test_for_deprecation_warning_supression()
{
  CompileTest t;
  RCPPUTILS_DEPRECATION_WARNING_OFF_START
  t.deprecated_function();
  RCPPUTILS_DEPRECATION_WARNING_OFF_STOP
}
