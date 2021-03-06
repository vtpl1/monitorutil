// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include <chrono>
#include <iostream>
#include <logging.h>
#include <thread>

#include "status_service.h"

int main(int /*argc*/, char const* /*argv*/[])
{
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  int i = 100000;
  std::cout << "Started" << std::endl;
  std::string name_of_app = "resource_monitor_test";
  std::string a = "session/";
  ::ray::RayLog::StartRayLog(name_of_app, ::ray::RayLogLevel::DEBUG, a);
  StatusService::getInstance();
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  std::atomic_uint_fast64_t& monitor_set_status = StatusService::setStatus(207, 1, 1);
  std::atomic_uint_fast64_t& monitor_set_status1 = StatusService::setStatus(207, 1, 2);
  while (i-- > 0) {
    monitor_set_status++;
    monitor_set_status1++;
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  std::cout << "End" << std::endl;

  return 0;
}
