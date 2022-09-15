#include <catch.hpp>
#include <fstream>
#include <logging.h>

#include "data_models.h"
// #include "status_service.h"

// TEST_CASE("test_status_service", "[status_service]")
// {
//   int i = 100000;
//   std::cout << "Started" << std::endl;
//   std::string name_of_app = "resource_monitor_test";
//   std::string a = "session/";
//   ::ray::RayLog::StartRayLog(name_of_app, ::ray::RayLogLevel::DEBUG, a);
//   StatusService::getInstance();
//   // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
//   std::atomic_uint_fast64_t& monitor_set_status = StatusService::setStatus(207, 1, 1);
//   std::atomic_uint_fast64_t& monitor_set_status1 = StatusService::setStatus(207, 1, 2);
//   std::atomic_uint_fast64_t& monitor_set_status2 = StatusService::setStatus(207, 1, 3);
//   while (i-- > 0) {
//     monitor_set_status++;
//     monitor_set_status1++;
//     monitor_set_status2++;
//     std::this_thread::sleep_for(std::chrono::milliseconds(1));
//   }
//   std::cout << "End" << std::endl;
// }

TEST_CASE("data_models_xml", "[data_models]")
{
  StatusConfig status_config;
  std::string filename = "./tests/status_config.json";
  std::ifstream ifs(filename);
  if (ifs.good()) {
    cereal::JSONInputArchive ar(ifs);
    ar(status_config);
  }
  ifs.close();

  std::stringstream ss;
  {
    cereal::JSONOutputArchive ar(ss);
    ar(status_config);
  }
  std::cout << "READ from local variable:==============\n" << ss.str() << std::endl;
}
