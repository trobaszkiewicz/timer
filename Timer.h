#include <thread>

class Timer
{
public:
  Timer()
  {
  }
  
  ~Timer()
  {
    task_thread_.join();
  }
  
  template<typename Task, typename Duration>
  void schedule(Task t, Duration d)
  {
    std::this_thread::sleep_for(d);
    std::thread task_thread([t, d, this]()
      {
        std::cout<<"going to sleep\n";
        std::this_thread::sleep_for(d);
        std::cout<<"BEFORE\n";
        t();
        std::cout<<"AFTER\n";
      }
    );
    task_thread_.swap(task_thread);
  }
  
  void cancel(void);

private : 
    std::thread task_thread_;
};
