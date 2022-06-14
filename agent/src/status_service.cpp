// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include "status_service.h"

constexpr int MONITOR_PORT_NUMBER = 29000;

StatusService::StatusService(std::string session_folder, std::string target_host_address, uint16_t target_port)
    : _session_folder(std::move(session_folder)), _target_host_address(std::move(target_host_address)),
      _target_port(target_port)
{
}

std::atomic_uint_fast64_t& setStatus(int16_t app_id, int16_t channel_id, uint64_t id);
StatusService& StatusService::getInstance() { return getInstance("session"); }
StatusService& StatusService::getInstance(std::string session_folder)
{
  return getInstance(std::move(session_folder), "127.0.0.1", MONITOR_PORT_NUMBER);
}
StatusService& StatusService::getInstance(std::string session_folder, std::string target_host_address,
                                          uint16_t target_port)
{
  static StatusService instance(std::move(session_folder), std::move(target_host_address), target_port);
  return instance;
}

std::atomic_uint_fast64_t& StatusService::setStatus(int16_t app_id, int16_t channel_id, uint64_t thread_id)
{
  return StatusService::setStatus(app_id, channel_id, thread_id, 0, 0, 0);
}
std::atomic_uint_fast64_t& StatusService::setStatusInternal(int16_t app_id, int16_t channel_id, uint64_t thread_id,
                                                            uint64_t process_id, uint64_t machine_id,
                                                            uint64_t cluster_id)
{
  std::vector<uint64_t> key;
  key.push_back(cluster_id);
  key.push_back(machine_id);
  key.push_back(process_id);
  key.push_back(thread_id);
  key.push_back(channel_id);
  key.push_back(app_id);
  auto it = _resource_map.find(key);
  if (it != _resource_map.end()) {
    it->second->value++;
    // it->second->last_update_time_in_ms.store(getCurrentTimeInMs());
    return it->second->value;
  }
  auto it1 = _resource_map.emplace(std::make_pair(key, std::move(std::make_unique<Status>())));
  return it1.first->second->value;
}
std::atomic_uint_fast64_t& StatusService::setStatus(int16_t app_id, int16_t channel_id, uint64_t thread_id,
                                                    uint64_t process_id, uint64_t machine_id, uint64_t cluster_id)
{
  return StatusService::getInstance().setStatusInternal(app_id, channel_id, thread_id, process_id, machine_id,
                                                        cluster_id);
}