// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************
#pragma once
#ifndef status_service_client_h
#define status_service_client_h

#include <thread>

#include "grpc_helper.h"
#include "status_service_grpc_caller.h"

class StatusServiceClient
{
private:
  std::atomic_bool _do_shutdown{false};
  bool _is_already_shutting_down{false};
  std::unique_ptr<std::thread> _thread;
  std::shared_ptr<StatusServiceGrpcCaller> _service_caller;

  const std::string _target_host_address;
  const uint16_t _target_port;

  void run();
  void stop();

public:
  StatusServiceClient(std::string target_host_address, uint16_t target_port);
  ~StatusServiceClient();
  void signal_to_stop();
  void set_status(std::shared_ptr<::resource::ClusterStatus> p_cluster_status, int64_t time_diff,
                  int64_t timestamp_sec);
  static StatusServiceClient& getInstance(std::string target_host_address, uint16_t target_port);
};
#endif // status_service_client_h
