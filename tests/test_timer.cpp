/**
 * Most of these tests actually violates unit tests principle to be "fast" as it uses timer dependent asynchronous waiting.
 * Some of the tests may fail on heavy loaded machine, as there is no guarantee that threads would be run in expected time slice.
 * Nevertheless I assume it's "good enough" to test Timer functionality.
 */

#include <gmock/gmock.h>
#include <future>

#include "timer/Timer.h"

TEST(TimerTest, SanityCheck) {
  EXPECT_TRUE(true);
}

TEST(TimerTest, schedule_once_executed) {
  Timer t;
  auto start = std::chrono::milliseconds(1);
  std::promise<int> promise;
  auto future = promise.get_future();
  t.schedule([&promise](){promise.set_value(123);}, start);

  auto result = future.get(); // wait for scheduled task to run
  ASSERT_THAT(result, testing::Eq(123));
}

TEST(TimerTest, schedule_once_cancelled) {
  Timer t;
  auto start = std::chrono::milliseconds(10);
  bool result = false;
  t.schedule([&result](){result = true;}, start);
  t.cancel();
  std::this_thread::sleep_for(std::chrono::milliseconds(20));

  ASSERT_FALSE(result);
}

TEST(TimerTest, cancel_without_scheduled_task) {
  Timer t;
  ASSERT_NO_THROW(t.cancel());
}

// optimistic assumption, about time of threads wakeup and their execution time !!
// It seems reasonable to assume that result would be in range [6,7]
TEST(TimerTest, schedule_periodic) {
  Timer t;
  auto start = std::chrono::milliseconds(1);
  auto period = std::chrono::milliseconds(10);
  int result = 0;
  t.schedule([&result](){result += 1;}, start, period);

  std::this_thread::sleep_for(std::chrono::milliseconds(65));
  ASSERT_THAT(result, testing::Ge(6));
  ASSERT_THAT(result, testing::Le(7));
}

TEST(TimerTest, destruct_timer_while_task_is_scheduled) {
  auto timer_function = []()
  {
    Timer t;
    auto start = std::chrono::milliseconds(10000);
    t.schedule([](){}, start);
  };
  ASSERT_NO_THROW(timer_function());
}

TEST(TimerTest, schedule_twice_throws) {
  Timer t;
  t.schedule([](){}, std::chrono::milliseconds(1));
  ASSERT_THROW(t.schedule([](){}, std::chrono::milliseconds(1)), std::logic_error);
}

std::promise<int> schedule_raw_function_promise;
void raw_function()
{
  schedule_raw_function_promise.set_value(5);
}

TEST(TimerTest, schedule_raw_function) {
  Timer t;
  auto promise = schedule_raw_function_promise.get_future();
  t.schedule(raw_function, std::chrono::milliseconds(1));
  auto i = promise.get();
  ASSERT_THAT(i, testing::Eq(5));
}

TEST(TimerTest, schedule_std_function) {
  Timer t;
  std::promise<int> promise;
  auto future = promise.get_future();
  std::function<void(void)> function = [&promise](){promise.set_value(123);};

  t.schedule(function, std::chrono::milliseconds(1));
  auto result = future.get();
  ASSERT_THAT(result, testing::Eq(123));
}
