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

#include <gtest/gtest.h>
#include <thread>
#include <atomic>
#include <chrono>

#include "rcpputils/mutex.hpp"
#include "rcpputils/thread.hpp"

using namespace std::chrono_literals;

TEST(test_mutex, pimutex_trylock) {
  rcpputils::PIMutex mutex;
  mutex.lock();

  EXPECT_FALSE(mutex.try_lock());

  mutex.unlock();
}

TEST(test_mutex, rpimutex_trylock) {
  rcpputils::RecursivePIMutex rmutex;
  rmutex.lock();

  EXPECT_TRUE(rmutex.try_lock());

  rmutex.unlock();
  rmutex.unlock();
}

TEST(test_mutex, pimutex_trylocktwice) {
  rcpputils::PIMutex mutex;

  EXPECT_TRUE(mutex.try_lock());
  EXPECT_FALSE(mutex.try_lock());

  mutex.unlock();
}

TEST(test_mutex, rpimutex_trylockmultiple) {
  rcpputils::RecursivePIMutex rmutex;

  const unsigned count = 20;

  for (unsigned i = 0; i < count; i++) {
    EXPECT_TRUE(rmutex.try_lock());
  }

  for (unsigned i = 0; i < count; i++) {
    rmutex.unlock();
  }
}

TEST(test_mutex, rpimutex_trylockthread) {
  rcpputils::RecursivePIMutex test_rmutex;
  std::atomic<int> result {-1};
  test_rmutex.lock();

  std::thread test_thread([&result, &test_rmutex]() {
      result = test_rmutex.try_lock();
    });
  test_thread.join();
  EXPECT_FALSE(result);

  test_rmutex.unlock();
}

TEST(test_mutex, pimutex_lockthread) {
  rcpputils::PIMutex test_mutex;
  test_mutex.lock();
  std::atomic<int> result {-1};

  std::thread test_thread([&result, &test_mutex]() {
      result = 0;
      test_mutex.lock();
      result = 1;  // this line should not be reached as long as the mutex is locked
    });
  std::this_thread::sleep_for(20ms);
  EXPECT_EQ(0, result);

  test_mutex.unlock();
  test_thread.join();
}

TEST(test_mutex, pimutex_priority_inversion) {
  rcpputils::PIMutex test_mutex;
  std::atomic<bool> end_low_prio_thread {false};
  std::atomic<bool> end_medium_prio_thread {false};
  const unsigned int cpu_bitmask = 1;  // allow only one cpu core to be used!

  // create low prio thread & take mutex
  std::thread low_prio_thread([&test_mutex, &end_low_prio_thread]() {
      test_mutex.lock();
      while (!end_low_prio_thread) {
        std::this_thread::sleep_for(1ms);
      }
      test_mutex.unlock();
    });
  if (rcpputils::configure_realtime_thread(
      low_prio_thread, THREAD_PRIORITY_LOW,
      cpu_bitmask) == false)
  {
    end_low_prio_thread = true;
    low_prio_thread.join();
    std::cerr << "THREAD_PRIORITY_LOW could not be set. Skipping testcase.\n";
    GTEST_SKIP();
    return;
  }

  // wait until mutex is taken by low prio thread
  while (test_mutex.try_lock()) {
    test_mutex.unlock();
    std::this_thread::sleep_for(1ms);
  }

  // create high prio thread & take mutex
  std::thread high_prio_thread([&test_mutex]() {
      test_mutex.lock();  // this call will block initially
      test_mutex.unlock();
    });
  EXPECT_TRUE(
    rcpputils::configure_realtime_thread(
      high_prio_thread,
      THREAD_PRIORITY_HIGH, cpu_bitmask)) << "THREAD_PRIORITY_HIGH could not be set.";

  // create medium priority thread that would block the low prio thread
  // if there is no priority inheritance
  std::thread medium_prio_thread([&end_medium_prio_thread]() {
      // create 100% workload on assigned cpu core
      while (!end_medium_prio_thread) {}
    });
  EXPECT_TRUE(
    rcpputils::configure_realtime_thread(
      medium_prio_thread,
      THREAD_PRIORITY_MEDIUM,
      cpu_bitmask)) << "THREAD_PRIORITY_MEDIUM could not be set.";

  // do the actual test: see if the low prio thread gets unblocked (through priority inheritance)
  end_low_prio_thread = true;
  std::this_thread::sleep_for(20ms);

  // if priority inheritance worked the mutex should not be locked anymore
  bool try_lock_result = test_mutex.try_lock();
  EXPECT_TRUE(try_lock_result) << "Mutex should not be locked anymore.";
  if (try_lock_result) {
    test_mutex.unlock();
  }

  // cleanup
  low_prio_thread.detach();
  high_prio_thread.detach();
  end_medium_prio_thread = true;
  medium_prio_thread.join();
}
