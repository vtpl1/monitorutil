// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include "set_status_service_run.h"
#include "logging.h"
#include "set_status_service.h"
#include "utilities.h"

SetStatusServiceRun::SetStatusServiceRun(std::string session_folder, std::string host_address, uint32_t port_number)
    : _session_folder(std::move(session_folder)), _host_address(std::move(host_address)), _port_number(port_number)
{
  _thread = std::make_unique<std::thread>(&SetStatusServiceRun::run, this);
}

SetStatusServiceRun::~SetStatusServiceRun()
{
  signal_to_stop();
  if (_thread->joinable()) {
    _thread->join();
  }
}

void SetStatusServiceRun::signal_to_stop()
{
  if (_server) {
    _server->Shutdown();
  }
}

void SetStatusServiceRun::run()
{
  RAY_LOG_INF << "Started";
  grpc::ServerBuilder builder;
  std::string listening_url = fmt::format("{}:{}", _host_address, _port_number);
  RAY_LOG_INF << "Trying to listen at: " << listening_url;
  builder.AddListeningPort(listening_url, grpc::InsecureServerCredentials());
  SetStatusService set_status_service(_session_folder);
  builder.RegisterService(&set_status_service);
  _server = builder.BuildAndStart();
  _server->Wait();
  RAY_LOG_INF << "Stopped";
}
