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
  int set_status();
};
#endif

#endif // status_service_grpc_caller_h
