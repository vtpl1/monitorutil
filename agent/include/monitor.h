// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef monitor_h
#define monitor_h
#include <atomic>
#include <map>
#include <memory>
#include <string>
#include <thread>

class Status
{
public:
  std::atomic_uint_fast64_t last_update_time_in_ms{0};
  std::atomic_uint_fast64_t last_value{0};
  std::atomic_uint_fast64_t value{0};
  Status() = default;
  ~Status() = default;
};
class Monitor
{
private:
  const std::string _target_host_address;
  const uint16_t _target_port;
  std::map<uint64_t, std::unique_ptr<Status>> _resource_map;
  bool _is_already_shutting_down{false};
  std::atomic_bool _do_shutdown{false};
  std::atomic_bool _is_internal_shutdown{false};
  std::unique_ptr<std::thread> _thread;
  std::string _session_folder;
  Monitor(std::string session_folder, std::string target_host_address, uint16_t target_port);
  ~Monitor();
  std::atomic_uint_fast64_t& setStatusInternal(uint64_t id);
  void run();

public:
  static std::atomic_uint_fast64_t& setStatus(int16_t app_id, int16_t channel_id, uint64_t id);
  static Monitor& getInstance();
  static Monitor& getInstance(std::string session_folder);
  static Monitor& getInstance(std::string session_folder, std::string target_host_address, uint16_t target_port);
};
#endif // monitor_h
