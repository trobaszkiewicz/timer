#include <chrono>
#include <thread>
#include <condition_variable>
#include <atomic>

class Timer
{
public:
  Timer() : cancelled_(false)
  {
  }

  ~Timer()
  {
    cancel();
  }

  template<typename Callable, typename Duration, typename Period = std::chrono::seconds>
  void schedule(Callable f, Duration start, Period period = std::chrono::seconds(0))
  {
    std::unique_lock<std::mutex> lock(m_);

    // this sync is needed in case thread that calls schedule() cancels before lock on m_ is acquired
    std::mutex sync_mutex;
    std::condition_variable sync_cond;
    std::unique_lock<std::mutex> sync_lock(sync_mutex);
    std::atomic<bool> ready;
    ready = false;

    std::thread t([this, start, period, f, &sync_mutex, &sync_cond, &ready]()
    {
      std::unique_lock<std::mutex> lock(m_);

      ready = true;
      sync_cond.notify_one();
      bool finish = false;

      auto wait_for = start;

      do
      {
        cv_.wait_for(lock, wait_for, [this](){return cancelled_;});

        if (!cancelled_)
        {
          f();
          wait_for = period;
        }
        else
          finish = true;

        cancelled_ = false;
      } while(period != std::chrono::seconds(0) && !finish);
    });

    execution_thread_.swap(t);

    lock.unlock();
    sync_cond.wait(sync_lock, [&ready](){return (bool)ready;}); // wait for execution_thread to start
  }

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
};
