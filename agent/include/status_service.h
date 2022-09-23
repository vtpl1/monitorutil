// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef status_service_h
#define status_service_h
#include <atomic>
#include <memory>
#include <string>
#include <thread>
#include <map>
#include <vector>

class Status
{
public:
  std::atomic_uint_fast64_t last_update_time_in_ms{0};
  std::atomic_uint_fast64_t last_value{0};
  std::atomic_uint_fast64_t value{0};
  Status() = default;
  ~Status() = default;
};
class StatusService
{
private:
  std::string _session_folder;
  const std::string _target_host_address;
  const uint16_t _target_port;
  std::map<std::vector<uint64_t>, std::unique_ptr<Status>> _resource_map;

  StatusService(std::string session_folder, std::string target_host_address, uint16_t target_port);
  ~StatusService();

  std::atomic_uint_fast64_t& setStatusInternal(int16_t app_id, int16_t channel_id, uint64_t thread_id,
                                               uint64_t process_id, uint64_t machine_id, uint64_t cluster_id);

  std::atomic_bool _do_shutdown{false};
  std::unique_ptr<std::thread> _thread;

public:
  static std::atomic_uint_fast64_t& setStatus(int16_t app_id, int16_t channel_id, uint64_t thread_id);
  static std::atomic_uint_fast64_t& setStatus(int16_t app_id, int16_t channel_id, uint64_t thread_id,
                                              uint64_t process_id, uint64_t machine_id, uint64_t cluster_id);
  static StatusService& getInstance();
  static StatusService& getInstance(std::string session_folder);
  static StatusService& getInstance(std::string session_folder, std::string target_host_address, uint16_t target_port);
  void run();
};

#endif // status_service_h
