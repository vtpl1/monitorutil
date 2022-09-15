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

#include "data_models.h"
#include "status_service_client.h"

class StatusAtomic
{
public:
  std::atomic_uint_fast64_t _cluster_id{0};
  std::atomic_uint_fast64_t _machine_id{0};
  std::atomic_uint_fast64_t _process_id{0};

  std::atomic_uint_fast64_t _app_id{0};
  std::atomic_uint_fast64_t _channel_id{0};
  std::atomic_uint_fast64_t _id{0};
  std::atomic_uint_fast64_t _value{0};
  std::atomic_uint_fast64_t _last_value{0};

  StatusAtomic(uint64_t cluster_id, uint64_t machine_id, uint64_t process_id, uint64_t app_id, uint64_t channel_id,
               uint64_t id);
  StatusAtomic(uint64_t cluster_id, uint64_t machine_id, uint64_t process_id, uint64_t app_id, uint64_t channel_id,
               uint64_t id, uint64_t value, uint64_t last_value);
  ~StatusAtomic() = default;
};

class StatusService
{
private:
  std::string _session_folder;
  const std::string _host_address;
  const uint32_t _port_number;

  std::atomic_bool _do_shutdown{false};
  std::unique_ptr<std::thread> _thread;

  std::map<size_t, std::unique_ptr<StatusAtomic>> _resource_map;
  StatusServiceClient& _status_service_client;
  StatusConfig _status_config;

  std::atomic_uint_fast64_t& _setStatusInternal(int16_t app_id, int16_t channel_id, uint64_t id, uint64_t process_id,
                                                uint64_t machine_id, uint64_t cluster_id);

  int64_t _get_current_time_in_ms();
  void _write_header_internal(std::shared_ptr<spdlog::logger> logger,
                              std::map<size_t, std::unique_ptr<StatusAtomic>>& resource_map);
  void _write_data(std::map<size_t, std::unique_ptr<StatusAtomic>>& _resource_map,
                   std::shared_ptr<spdlog::logger> logger, int64_t& last_write_time, size_t& last_map_size);

public:
  StatusService(std::string session_folder, std::string host_address, uint32_t port_number);
  ~StatusService();

  static StatusService& getInstance(std::string session_folder, std::string host_address, uint32_t port_number);
  std::atomic_uint_fast64_t& setStatus(int16_t app_id, int16_t channel_id, uint64_t thread_id);

  void run();
  void send_to_server(std::shared_ptr<::resource::ClusterStatus> p_cluster_status, int64_t time_diff,
                      int64_t timestamp_sec);
  StatusConfig load_status_config(std::string config_file);
};

#endif // status_service_h
