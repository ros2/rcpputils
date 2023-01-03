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

TEST(test_mutex, rpimutex_trylocktwice) {
  rcpputils::RecursivePIMutex rmutex;

  EXPECT_TRUE(rmutex.try_lock());
  EXPECT_TRUE(rmutex.try_lock());

  rmutex.unlock();
  rmutex.unlock();
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
      result = 1; // this line should not be reached
    });
  std::this_thread::sleep_for(20ms);
  test_thread.detach();
  EXPECT_EQ(0, result);

  test_mutex.unlock();
}

#ifndef _WIN32

#include <pthread.h>

TEST(test_mutex, pimutex_priority_inversion) {
  // ToDo
  EXPECT_FALSE(true);
}

TEST(test_mutex, rpimutex_priority_inversion) {
  // ToDo
  EXPECT_FALSE(true);
}

#endif  // _WIN32
