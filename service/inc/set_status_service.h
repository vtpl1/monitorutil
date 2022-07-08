// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef set_status_service_h
#define set_status_service_h
#include <string>

class SetStatusService
{
private:
  std::string _application_installation_directory;
  std::string _config_directory;
  std::string _data_directory;

public:
  SetStatusService(std::string application_installation_directory, std::string config_directory,
                   std::string data_directory);
  ~SetStatusService();
};



SetStatusService::~SetStatusService() {}

#endif // set_status_service_h
