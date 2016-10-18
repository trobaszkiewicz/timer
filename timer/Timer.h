#include <chrono>
#include <thread>
#include <condition_variable>
#include <atomic>

/**
 * Represents timer, that can execute tasks with delay.
 * It can execute one time only task, or do it cyclically every some specific time period.
 * After task is scheduled for execution (schedule() method), no other task can be scheduled for the same Timer instance.
 * Canceling task is also supported via cancel() method.
 * This class is noncopyable.
 * Scheduling a task spawns a thread.
 */
class Timer
{
public:
  Timer() : cancelled_(false), scheduled_(false)
  {
  }

  ~Timer()
  {
    cancel();
  }

  Timer(const Timer&) = delete;
  Timer& operator =(const Timer&) = delete;

  /**
   * Schedules Callable for execution
   * @tparam task
   * @tparam start_after time after which the task will be executed
   * @tparam period if != 0 enables cyclic execution of task
   */
  template<typename Callable, typename Duration, typename Period = std::chrono::seconds>
  void schedule(Callable task, Duration start_after, Period period = std::chrono::seconds(0))
  {
    std::unique_lock<std::mutex> lock(m_);
    if (scheduled_)
      throw(std::logic_error("timer already scheduled"));

    scheduled_ = true;

    // this sync is needed in case thread that calls schedule() cancels before lock on m_ is acquired
    std::mutex sync_mutex;
    std::condition_variable sync_cond;
    std::unique_lock<std::mutex> sync_lock(sync_mutex);
    std::atomic<bool> ready;
    ready = false;

    std::thread t([this, start_after, period, task, &sync_mutex, &sync_cond, &ready]()
    {
      std::unique_lock<std::mutex> lock(m_);

      ready = true;
      sync_cond.notify_one();
      bool should_finish = false;

      auto wait_for = start_after;

      do
      {
        cv_.wait_for(lock, wait_for, [this](){return cancelled_;});

        if (!cancelled_)
        {
          task();
          wait_for = period;
        }
        else
          should_finish = true;

        cancelled_ = false;
      } while(period != std::chrono::seconds(0) && !should_finish);
    });

    execution_thread_.swap(t);

    lock.unlock();
    sync_cond.wait(sync_lock, [&ready](){return (bool)ready;}); // wait for execution_thread to start_after
  }

  /**
   * Cancels scheduled task. If task started to run blocks till it finishes
   */
  void cancel()
  {
    std::unique_lock<std::mutex> lock(m_);
    if (execution_thread_.joinable())
    {
      cancelled_ = true;
      cv_.notify_one();

      lock.unlock();
      execution_thread_.join();
    }
  }
private:
  std::mutex m_;
  std::condition_variable cv_;
  bool cancelled_;
  std::thread execution_thread_;
  bool scheduled_;
};
