// *****************************************************
//	Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef sys_timestamp_h
#define sys_timestamp_h
#include <cstdint>

#define ONE_SEC_MILLIS 1000
#define ONE_MIN_MILLIS 60000
// #define ONE_HOUR_MILLIS 3600000
// #define ONE_DAY_SECS 86400
#define ONE_DAY_MILLIS 86400000

#define N_DAY_MILLIS(n) (ONE_DAY_MILLIS * n)
// #define N_HOUR_MILLIS(n) (ONE_HOUR_MILLIS * n)
#define N_MIN_MILLIS(n) (ONE_MIN_MILLIS * n)
#define N_SEC_MILLIS(n) (ONE_SEC_MILLIS * n)
// #define MILLIS_TO_MIN(n) (n / 60000)

namespace vtpl
{
namespace timestamp
{
class sys_timestamp
{
public:
  static int64_t get();

private:
  sys_timestamp() = default;
  ~sys_timestamp() = default;
  static sys_timestamp& get_instance();
  int64_t _get();
};
} // namespace timestamp
} // namespace vtpl

#endif // !sys_timestamp_h
