// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef set_status_service_h
#define set_status_service_h

#include <grpc_helper.h>
#include <string>
#include <thread>

#include "logging.h"

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
               uint64_t id, uint64_t value, uint64_t last_value);
  StatusAtomic(const StatusAtomic& o);
  StatusAtomic& operator=(const StatusAtomic& o);
  ~StatusAtomic() = default;
};

class SetStatusService final : public resource::StatusService::Service
{
private:
  std::string _session_folder;
  std::map<size_t, std::unique_ptr<StatusAtomic>> _m_list;
  std::atomic_bool _do_shutdown{false};
  std::unique_ptr<std::thread> _thread;

  std::atomic_uint_fast64_t _time_diff{0};
  std::atomic_uint_fast64_t _timestamp_sec{0};

  void _write_data(std::shared_ptr<spdlog::logger> logger, size_t& last_map_size);
  void _write_header_internal(std::shared_ptr<spdlog::logger> logger);

public:
  SetStatusService(std::string session_folder);
  ~SetStatusService();
  grpc::Status SetStatus(grpc::ServerContext* context, const resource::SetStatusRequest* request,
                         resource::SetStatusResponse* response);
  void run();
};

#endif // set_status_service_h
