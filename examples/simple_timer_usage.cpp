#include <chrono>
//#include <iostream>

#include "timer/Timer.h"

int main(int argc, char** argv)
{
  Timer t;
  t.schedule([](){/*std::cout<<"TADA!!\n";*/}, std::chrono::milliseconds(200), std::chrono::seconds(2));
  std::this_thread::sleep_for(std::chrono::seconds(10));
  t.cancel();
  //std::this_thread::sleep_for(std::chrono::seconds(3));
  return 0;
}
