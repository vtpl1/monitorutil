// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include "status_service_grpc_caller.h"

StatusServiceGrpcCaller::StatusServiceGrpcCaller(std::shared_ptr<grpc::ChannelInterface> channel)
    : _stub(resource::StatusService::NewStub(channel))
{
}
int StatusServiceGrpcCaller::set_status()
{
  ::grpc::ClientContext context;
  resource::SetStatusRequest request;
  resource::SetStatusResponse response;
  return 0;
}