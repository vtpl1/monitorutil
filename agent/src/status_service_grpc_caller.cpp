// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include "status_service_grpc_caller.h"
#include "logging.h"

StatusServiceGrpcCaller::StatusServiceGrpcCaller(std::shared_ptr<grpc::ChannelInterface> channel)
    : _stub(resource::StatusService::NewStub(channel))
{
}

int StatusServiceGrpcCaller::set_status(std::shared_ptr<::resource::ClusterStatus> p_cluster_status, int64_t time_diff,
                                        int64_t timestamp_sec)
{
  grpc::ClientContext context;
  resource::SetStatusRequest request;
  resource::ClusterStatus* cluster_status = request.mutable_cluster_status();
  *cluster_status = *p_cluster_status;
  request.set_time_diff(time_diff);
  request.set_timestamp_sec(timestamp_sec);

  resource::SetStatusResponse response;
  grpc::Status status;
  status = _stub->SetStatus(&context, request, &response);
  size_t key = 0;
  if (!status.ok()) {
    std::stringstream err;
    err << "Error: ";
    err << status.error_code();
    RAY_LOG_ERR << err.str();
  }
  return key;
}