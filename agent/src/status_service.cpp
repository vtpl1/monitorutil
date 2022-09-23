// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include <chrono>
#include <fmt/chrono.h>
#include <logging.h>

#include "status_service.h"

constexpr int MONITOR_PORT_NUMBER = 29000;
constexpr int max_sleep_upto_sec = 5;

StatusService::StatusService(std::string session_folder, std::string target_host_address, uint16_t target_port)
    : _session_folder(std::move(session_folder)), _target_host_address(std::move(target_host_address)),
      _target_port(target_port)
{
  _thread = std::make_unique<std::thread>(&StatusService::run, this);
}

StatusService::~StatusService()
{
  _do_shutdown = true;
  _thread->join();
  for (auto&& it : _resource_map) {
    it.second.reset();
    it.second = nullptr;
  }
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

int64_t getCurrentTimeInMs()
{
  return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
      .count();
}

void write_header_internal(std::shared_ptr<spdlog::logger> logger,
                           std::map<std::vector<uint64_t>, std::unique_ptr<Status>>& resource_map)
{
  {
    std::stringstream ss;
    for (auto&& it : resource_map) {
      ss << "app.chn.id    fps|";
    }
    write_header(logger, ss.str());
  }

  {
    std::stringstream ss;
    for (auto&& it : resource_map) {
      ss << fmt::format("{:03}.{:03}.{:03}   fps|", it.first.at(5), it.first.at(4), it.first.at(3));
    }
    write_log(logger, ss.str());
  }
}

void write_data(std::map<std::vector<uint64_t>, std::unique_ptr<Status>>& _resource_map,
                std::shared_ptr<spdlog::logger> logger, int64_t& last_write_time, size_t& last_map_size)
{
  if (last_map_size != _resource_map.size()) {
    write_header_internal(logger, _resource_map);
    last_map_size = _resource_map.size();
  }

  int64_t date_timestamp = getCurrentTimeInMs();
  std::stringstream ss;
  int64_t time_diff = date_timestamp - last_write_time;
  last_write_time = date_timestamp;
  if (time_diff <= 0) {
    return;
  }
  for (auto&& it : _resource_map) {
    float diff = ((static_cast<float>(it.second->value) - static_cast<float>(it.second->last_value)) * 1000.0F /
                  static_cast<float>(time_diff));

    // uint64_t unique_id = get_unique_id(it.first.at(5), it.first.at(4), it.first.at(3));
    // l_p_th->set_last_updated_in_ms(it.second->last_update_time_in_ms);
    it.second->last_value.store(it.second->value);
    ss << fmt::format("{:>17.{}f}|", diff, 1);
  }
  write_log(logger, ss.str());
  // send_to_server(s, machine_status, date_timestamp, _target_host_address, _target_port);
}

void StatusService::run()
{
  int sleep_upto_sec = max_sleep_upto_sec;
  int iteration_counter = 0;
  int fps_log_counter = 0;
  auto logger = get_logger_st(_session_folder, "fps");
  size_t last_map_size = 0;
  int64_t last_write_time = getCurrentTimeInMs();

  while (!_do_shutdown) {
    if (sleep_upto_sec > 0) {
      sleep_upto_sec--;
    } else {
      sleep_upto_sec = max_sleep_upto_sec;
      write_data(_resource_map, logger, last_write_time, last_map_size);
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  write_data(_resource_map, logger, last_write_time, last_map_size);
}