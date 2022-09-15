// *****************************************************
//	Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include <chrono>

#include "sys_timestamp.h"

vtpl::timestamp::sys_timestamp& vtpl::timestamp::sys_timestamp::get_instance()
{
  static vtpl::timestamp::sys_timestamp instance;
  return instance;
}
int64_t vtpl::timestamp::sys_timestamp::get() { return get_instance()._get(); }

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
int64_t vtpl::timestamp::sys_timestamp::_get()
{
  int64_t current_timestamp =
      std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
          .count();
  return current_timestamp;
}
