// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************
#pragma once
#ifndef status_service_grpc_caller_h
#define status_service_grpc_caller_h

#if 1
#include <memory>

#include "grpc_helper.h"

class StatusServiceGrpcCaller
{
private:
  std::unique_ptr<resource::StatusService::Stub> _stub;

public:
  StatusServiceGrpcCaller(std::shared_ptr<grpc::ChannelInterface> channel);
  ~StatusServiceGrpcCaller() = default;
  int set_status(std::shared_ptr<::resource::ClusterStatus> p_cluster_status, int64_t time_diff, int64_t timestamp_sec);
  uint32_t get_listening_port();
};
#endif

#endif // status_service_grpc_caller_h
