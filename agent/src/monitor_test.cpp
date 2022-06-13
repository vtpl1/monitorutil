// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include <chrono>
#include <iostream>
#include <logging.h>
#include <thread>

#include "monitor.h"

int main(int /*argc*/, char const* /*argv*/[])
{
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  int i = 15000;
  std::cout << "Started" << std::endl;
  std::string name_of_app = "resource_monitor_test";
  std::string a = "session/";
  ::ray::RayLog::StartRayLog(name_of_app, ::ray::RayLogLevel::DEBUG, a);
  Monitor::getInstance();
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  std::atomic_uint_fast64_t& monitor_set_status = Monitor::setStatus(207, 1, 1);
  while (i-- > 0) {
    // std::cout << "put" << std::endl;

    // Monitor::setStatus(207, 1, 1);
    monitor_set_status++;
    std::this_thread::sleep_for(std::chrono::microseconds(1000 - 10));
  }
  std::cout << "End" << std::endl;

  return 0;
}
