// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef monitor_service_grpc_caller_h
#define monitor_service_grpc_caller_h
#if 1
#include <memory>

#include "protobuf_grpc_helper.h"
class MonitorServiceGrpcCaller
{
private:
  std::unique_ptr<resource::MonitorService::Stub> _stub;

public:
  MonitorServiceGrpcCaller(std::shared_ptr<grpc::ChannelInterface> channel);
  ~MonitorServiceGrpcCaller() = default;
  int set_status();
};
#endif
#endif // monitor_service_grpc_caller_h
