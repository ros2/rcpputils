// Copyright (c) 2025, cellumation GmbH
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

#ifndef RCPPUTILS__COMPILE_WARNINGS_HPP_
#define RCPPUTILS__COMPILE_WARNINGS_HPP_

#if !defined(_WIN32)
#define RCPPUTILS_DEPRECATION_WARNING_OFF_START \
  _Pragma("GCC diagnostic push") \
  _Pragma("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
#define RCPPUTILS_DEPRECATION_WARNING_OFF_STOP \
  _Pragma("GCC diagnostic pop")
#else  // !defined(_WIN32)
#define RCPPUTILS_DEPRECATION_WARNING_OFF_START \
  _Pragma("warning(push)") \
  _Pragma("warning(disable: 4996)")
#define RCPPUTILS_DEPRECATION_WARNING_OFF_STOP \
  _Pragma("warning(pop)")
#endif

#endif  // RCPPUTILS__COMPILE_WARNINGS_HPP_
