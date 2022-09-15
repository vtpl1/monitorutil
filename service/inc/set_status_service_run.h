// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef set_status_service_run_h
#define set_status_service_run_h

#include <grpcpp/server_builder.h>
#include <memory>
#include <thread>

class SetStatusServiceRun
{
private:
  std::string _session_folder;

  std::string _host_address;
  uint32_t _port_number;

  std::unique_ptr<std::thread> _thread;
  std::unique_ptr<grpc::Server> _server;

public:
  SetStatusServiceRun(std::string session_folder, std::string host_address, uint32_t port_number);
  ~SetStatusServiceRun();

  void run();
  void signal_to_stop();
};

#endif // set_status_service_run_h
