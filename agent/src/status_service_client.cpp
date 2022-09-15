// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include <grpcpp/grpcpp.h>

#include "logging.h"
#include "status_service_client.h"

StatusServiceClient::StatusServiceClient(std::string target_host_address, uint16_t target_port)
    : _target_host_address(std::move(target_host_address)), _target_port(target_port)
{
  _thread = std::make_unique<std::thread>(&StatusServiceClient::run, this);
}

StatusServiceClient& StatusServiceClient::getInstance(std::string target_host_address, uint16_t target_port)
{
  static StatusServiceClient instance(std::move(target_host_address), target_port);
  return instance;
}

StatusServiceClient::~StatusServiceClient() { stop(); }

void StatusServiceClient::stop()
{
  // NOLINTNEXTLINE(clang-analyzer-optin.cplusplus.VirtualCall)
  signal_to_stop();
  if (_thread) {
    if (_thread->joinable()) {
      _thread->join();
    }
  }
}

void StatusServiceClient::signal_to_stop()
{
  if (_is_already_shutting_down) {
    return;
  }
  _is_already_shutting_down = true;
  _do_shutdown = true;
}

void StatusServiceClient::set_status(std::shared_ptr<::resource::ClusterStatus> p_cluster_status, int64_t time_diff,
                                     int64_t timestamp_sec)
{
  _service_caller->set_status(std::move(p_cluster_status), time_diff, timestamp_sec);
}

void StatusServiceClient::run()
{
  RAY_LOG_INF << "Thread Started for StatusServiceClient";
  std::string url = fmt::format("localhost:{}", _target_port);
  RAY_LOG_INF << "Trying to connect: " << url;
  std::shared_ptr<grpc::Channel> channel = grpc::CreateChannel(url, grpc::InsecureChannelCredentials());

  while (!_do_shutdown) {
    try {
      _service_caller = std::make_shared<StatusServiceGrpcCaller>(channel);

      // while (!_do_shutdown) {

      //   std::this_thread::sleep_for(std::chrono::seconds(1));
      // }

    } catch (const std::exception& e) {
      RAY_LOG_INF << "StatusServiceGrpcCaller error: " << e.what();
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}
