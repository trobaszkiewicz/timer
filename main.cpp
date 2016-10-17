#include <iostream>
#include <chrono>

#include "Timer.h"

int main(int argc, char** argv)
{
  Timer t;
  t.schedule([](){std::cout<<"DONE\n";}, std::chrono::seconds(1));
  std::this_thread::sleep_for(std::chrono::seconds(2));
}
