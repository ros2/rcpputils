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
  std::this_thread::sleep_for(10ms);
  EXPECT_EQ(0, result);

  test_mutex.unlock();
  test_thread.join();
}

template <class MutexClass>
void priority_inheritance_test() {
  MutexClass test_mutex;
  std::atomic<bool> end_low_prio_thread {false};
  std::atomic<bool> end_medium_prio_thread {false};
  const unsigned int cpu_bitmask = 1;  // allow only one cpu core to be used!

  // create low prio thread & take mutex
  std::thread low_prio_thread([&test_mutex, &end_low_prio_thread]() {
      std::cout << "Low prio thread starts.\n" << std::flush;
      test_mutex.lock();
      std::cout << "Low prio thread locked.\n" << std::flush;
      while (!end_low_prio_thread) {
        std::this_thread::sleep_for(1ms);
      }
      test_mutex.unlock();
      std::cout << "Low prio thread unlocked and ends.\n" << std::flush;
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
  std::cout << "Test thread try_lock failed as expected.\n" << std::flush;

  // create high prio thread & take mutex
  std::thread high_prio_thread([&test_mutex]() {
      std::cout << "High prio thread starts.\n" << std::flush;
      test_mutex.lock();  // this call will block initially
      std::cout << "High prio thread locked.\n" << std::flush;
      test_mutex.unlock();
      std::cout << "High prio thread unlocked and ends.\n" << std::flush;
    });
  EXPECT_TRUE(
    rcpputils::configure_realtime_thread(
      high_prio_thread,
      THREAD_PRIORITY_HIGH, cpu_bitmask)) << "THREAD_PRIORITY_HIGH could not be set.";

  // create medium priority thread that would block the low prio thread
  // if there is no priority inheritance
  std::atomic<bool> medium_thread_started {false};
  std::thread medium_prio_thread([&end_medium_prio_thread, &medium_thread_started]() {
      std::cout << "Medium prio thread starts.\n" << std::flush;
      medium_thread_started = true;
      // create 100% workload on assigned cpu core
      while (!end_medium_prio_thread) {}
      std::cout << "Medium prio thread ends.\n" << std::flush;
    });
  EXPECT_TRUE(
    rcpputils::configure_realtime_thread(
      medium_prio_thread,
      THREAD_PRIORITY_MEDIUM,
      cpu_bitmask)) << "THREAD_PRIORITY_MEDIUM could not be set.";
  while (medium_thread_started == false) {
    std::this_thread::sleep_for(1ms);
  }

  // do the actual test: see if the low prio thread gets unblocked (through priority inheritance)
  std::cout << "Signalling end low prio thread.\n" << std::flush;
  end_low_prio_thread = true;

  // if priority inheritance worked the mutex should not be locked anymore
  bool try_lock_result;
  int count = 0;
  while((try_lock_result = test_mutex.try_lock()) == false)
  {
    std::this_thread::sleep_for(1ms);
    if(count++ >= 20)
    {
      EXPECT_TRUE(try_lock_result) << "Mutex should not be locked anymore.";
      break;
    }
  }
  if (try_lock_result) {
    test_mutex.unlock();
  }

  // cleanup
  std::cout << "Signalling end medium prio thread.\n" << std::flush;
  end_medium_prio_thread = true;
  medium_prio_thread.join();
  high_prio_thread.join();
  low_prio_thread.join();
}

TEST(test_mutex, pimutex_priority_inversion) {
  priority_inheritance_test<rcpputils::PIMutex>();
}

TEST(test_mutex, rpimutex_priority_inversion) {
  priority_inheritance_test<rcpputils::RecursivePIMutex>();
}
