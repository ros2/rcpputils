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

#ifndef RCPPUTILS__THREAD_SAFETY_ANNOTATIONS_H_
#define RCPPUTILS__THREAD_SAFETY_ANNOTATIONS_H_

#include <mutex>

// Enable thread safety attributes only with clang.
// The attributes can be safely erased when compiling with other compilers.
// Prefixing all macros to avoid potential conflict with other projects

#if defined(__clang__) && (!defined(SWIG))
#define RCPPUTILS_THREAD_ANNOTATION_ATTRIBUTE__(x)   __attribute__((x))
#else
#define RCPPUTILS_THREAD_ANNOTATION_ATTRIBUTE__(x)   // no-op
#endif

// libcxx does not define this operator, needed for negative capabilities
// Here until someone has a better idea
inline const std::mutex & operator!(const std::mutex & a)
{
  return a;
}

#define RCPPUTILS_CAPABILITY(x) \
  RCPPUTILS_THREAD_ANNOTATION_ATTRIBUTE__(capability(x))

#define RCPPUTILS_SCOPED_CAPABILITY \
  RCPPUTILS_THREAD_ANNOTATION_ATTRIBUTE__(scoped_lockable)

#define RCPPUTILS_GUARDED_BY(x) \
  RCPPUTILS_THREAD_ANNOTATION_ATTRIBUTE__(guarded_by(x))

#define RCPPUTILS_PT_GUARDED_BY(x) \
  RCPPUTILS_THREAD_ANNOTATION_ATTRIBUTE__(pt_guarded_by(x))

#define RCPPUTILS_ACQUIRED_BEFORE(...) \
  RCPPUTILS_RCPPUTILS_THREAD_ANNOTATION_ATTRIBUTE__(acquired_before(__VA_ARGS__))

#define RCPPUTILS_ACQUIRED_AFTER(...) \
  RCPPUTILS_THREAD_ANNOTATION_ATTRIBUTE__(acquired_after(__VA_ARGS__))

#define RCPPUTILS_REQUIRES(...) \
  RCPPUTILS_THREAD_ANNOTATION_ATTRIBUTE__(requires_capability(__VA_ARGS__))

#define RCPPUTILS_REQUIRES_SHARED(...) \
  RCPPUTILS_THREAD_ANNOTATION_ATTRIBUTE__(requires_shared_capability(__VA_ARGS__))

#define RCPPUTILS_ACQUIRE(...) \
  RCPPUTILS_THREAD_ANNOTATION_ATTRIBUTE__(acquire_capability(__VA_ARGS__))

#define RCPPUTILS_ACQUIRE_SHARED(...) \
  RCPPUTILS_THREAD_ANNOTATION_ATTRIBUTE__(acquire_shared_capability(__VA_ARGS__))

#define RCPPUTILS_RELEASE(...) \
  RCPPUTILS_THREAD_ANNOTATION_ATTRIBUTE__(release_capability(__VA_ARGS__))

#define RCPPUTILS_RELEASE_SHARED(...) \
  RCPPUTILS_THREAD_ANNOTATION_ATTRIBUTE__(release_shared_capability(__VA_ARGS__))

#define RCPPUTILS_TRY_ACQUIRE(...) \
  RCPPUTILS_THREAD_ANNOTATION_ATTRIBUTE__(try_acquire_capability(__VA_ARGS__))

#define RCPPUTILS_TRY_ACQUIRE_SHARED(...) \
  RCPPUTILS_THREAD_ANNOTATION_ATTRIBUTE__(try_acquire_shared_capability(__VA_ARGS__))

#define RCPPUTILS_EXCLUDES(...) \
  RCPPUTILS_THREAD_ANNOTATION_ATTRIBUTE__(locks_excluded(__VA_ARGS__))

#define RCPPUTILS_ASSERT_CAPABILITY(x) \
  RCPPUTILS_THREAD_ANNOTATION_ATTRIBUTE__(assert_capability(x))

#define RCPPUTILS_ASSERT_SHARED_CAPABILITY(x) \
  RCPPUTILS_THREAD_ANNOTATION_ATTRIBUTE__(assert_shared_capability(x))

#define RCPPUTILS_RETURN_CAPABILITY(x) \
  RCPPUTILS_THREAD_ANNOTATION_ATTRIBUTE__(lock_returned(x))

#define RCPPUTILS_NO_THREAD_SAFETY_ANALYSIS \
  RCPPUTILS_THREAD_ANNOTATION_ATTRIBUTE__(no_thread_safety_analysis)

#endif  // RCPPUTILS__THREAD_SAFETY_ANNOTATIONS_H_
