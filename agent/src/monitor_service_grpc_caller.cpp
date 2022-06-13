// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include "monitor_service_grpc_caller.h"
MonitorServiceGrpcCaller::MonitorServiceGrpcCaller(std::shared_ptr<grpc::ChannelInterface> channel)
    : _stub(resource::MonitorService::NewStub(channel))
{
}
int MonitorServiceGrpcCaller::set_status()
{
  ::grpc::ClientContext context;
  resource::SetStatusRequest request;
  resource::SetStatusResponse response;
  return 0;
}