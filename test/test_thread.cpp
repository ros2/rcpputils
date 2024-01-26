// Copyright 2023 eSOL Co.,Ltd.
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

#include <gtest/gtest.h>

#include <pthread.h>
#include <atomic>

#include "rcpputils/thread.hpp"

using rcpputils::Thread;
using rcpputils::ThreadId;
using rcpputils::ThreadAttribute;
using rcpputils::SchedPolicy;
using rcpputils::CpuSet;

#if __linux__
using rcpputils::thread::detail::sched_policy_explicit_bit;
#endif

TEST(test_thread, basic_run) {
  std::atomic<bool> pass = false;
  Thread thread(
    [&] {
      pass = true;
    });
  thread.join();
  EXPECT_TRUE(pass);
}

TEST(test_thread, run_with_attribtue) {
  std::atomic<bool> pass = false;
  ThreadAttribute attr;
#if __linux__
  pthread_t parent_thread = pthread_self();
  {
    sched_param param;
    param.sched_priority = 0;
    int r = pthread_setschedparam(parent_thread, SCHED_OTHER, &param);
    ASSERT_EQ(r, 0);

    attr.set_sched_policy(SchedPolicy::batch);
  }
#endif
  Thread thread(
    attr,
    [&] {
#if __linux__
      int policy;
      sched_param param;
      pthread_t sub_thread = pthread_self();
      int r = pthread_getschedparam(sub_thread, &policy, &param);
      SchedPolicy rclcpp_policy = SchedPolicy(policy | sched_policy_explicit_bit);
      EXPECT_EQ(0, r);
      EXPECT_EQ(SchedPolicy::batch, rclcpp_policy);
      EXPECT_FALSE(pthread_equal(parent_thread, sub_thread));
#endif
      pass = true;
    });
  thread.join();
  EXPECT_TRUE(pass);
}

TEST(thread, attribute) {
  ThreadAttribute attr;

  std::size_t stack_size = attr.get_stack_size();
  EXPECT_NE(0, stack_size);
  std::size_t increased_stack_size = stack_size + 4 * 1024 * 1024;
  attr.set_stack_size(increased_stack_size);
  EXPECT_EQ(increased_stack_size, attr.get_stack_size());

  // copy
  {
    ThreadAttribute attr2 = attr;
    EXPECT_EQ(increased_stack_size, attr2.get_stack_size());
  }
  // swap
  {
    ThreadAttribute attr2;
#if __linux__
    std::size_t stack_size2 = attr2.get_stack_size();
    const std::size_t increased_stack_size2 = stack_size2 + 2 * 1024 * 1024;
    attr2.set_stack_size(increased_stack_size2);
#endif  // __linux__
    swap(attr, attr2);
    EXPECT_EQ(increased_stack_size, attr2.get_stack_size());
    EXPECT_EQ(increased_stack_size2, attr.get_stack_size());
  }
  // convert from rcutils_thread_attr_t
  {
    rcutils_thread_attr_t rcutils_attr;
    rcutils_attr.core_affinity = rcutils_get_zero_initialized_thread_core_affinity();
    rcutils_attr.scheduling_policy = RCUTILS_THREAD_SCHEDULING_POLICY_FIFO;
    rcutils_attr.priority = 42;
    rcutils_attr.tag = NULL;

    attr.set_rcutils_thread_attribute(rcutils_attr);

    EXPECT_EQ(0, attr.get_affinity().count());
    EXPECT_EQ(SchedPolicy::fifo, attr.get_sched_policy());
    EXPECT_EQ(42, attr.get_priority());
  }
}


TEST(attribute, cpu_set) {
#if __linux__
  std::size_t n = CpuSet::num_processors();
  CpuSet cpu_set;
  for (std::size_t i = 0; i < n; ++i) {
    if (i % 3 == 0) {
      cpu_set.set(i);
    }
  }
  for (std::size_t i = 0; i < n; ++i) {
    EXPECT_EQ(i % 3 == 0, cpu_set.is_set(i));
  }

  // copy
  {
    CpuSet cpu_set2 = cpu_set;
    for (std::size_t i = 0; i < n; ++i) {
      EXPECT_EQ(cpu_set.is_set(i), cpu_set2.is_set(i));
    }
  }

  // swap
  {
    CpuSet cpu_set2;
    for (std::size_t i = 0; i < n; ++i) {
      if (i % 2 == 0) {
        cpu_set2.set(i);
      }
    }
    swap(cpu_set, cpu_set2);
    for (std::size_t i = 0; i < n; ++i) {
      EXPECT_EQ(i % 2 == 0, cpu_set.is_set(i));
      EXPECT_EQ(i % 3 == 0, cpu_set2.is_set(i));
    }
  }

  // convert from rcutils_thread_core_affinity_t affinity;
#else
  GTEST_SKIP();
#endif
}
