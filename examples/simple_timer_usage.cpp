#include <chrono>
#include <iostream>

#include "timer/Timer.h"

int main(int argc, char** argv)
{
  Timer one_time_timer, periodic_timer, cancelled_timer;

  one_time_timer.schedule([](){std::cout<<"One time task is just running\n";}, std::chrono::milliseconds(200));

  cancelled_timer.schedule([](){std::cout<<"Cancelled task - this is never run\n";}, std::chrono::milliseconds(1000));

  periodic_timer.schedule([](){
                    static int n = 0;
                    n++;
                    std::cout<<"Periodic task is running for the " << n << " time\n";
                  },
                  std::chrono::milliseconds(200),
                  std::chrono::seconds(2));

  cancelled_timer.cancel();

  std::this_thread::sleep_for(std::chrono::seconds(10));

  return 0;
}
