// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include <absl/hash/hash.h>
#include <bits/stdc++.h>
#include <chrono>
#include <fmt/chrono.h>
#include <fstream>
#include <logging.h>

#include "status_service.h"
#include "status_service_grpc_caller.h"
#include "utilities.h"

const std::string CONFIG_FILE = "status_config.json";
constexpr int max_sleep_upto_sec = 10;

StatusAtomic::StatusAtomic(uint64_t cluster_id, uint64_t machine_id, uint64_t process_id, uint64_t app_id,
                           uint64_t channel_id, uint64_t id)
    : StatusAtomic(cluster_id, machine_id, process_id, app_id, channel_id, id, 0, 0)
{
}

StatusAtomic::StatusAtomic(uint64_t cluster_id, uint64_t machine_id, uint64_t process_id, uint64_t app_id,
                           uint64_t channel_id, uint64_t id, uint64_t value, uint64_t last_value)
    : _cluster_id(cluster_id), _machine_id(machine_id), _process_id(process_id), _app_id(app_id),
      _channel_id(channel_id), _id(id), _value(value), _last_value(last_value)
{
}

StatusService::StatusService(std::string session_folder, std::string host_address, uint32_t port_number)
    : _session_folder(std::move(session_folder)), _host_address(std::move(host_address)), _port_number(port_number),
      _status_service_client(StatusServiceClient::getInstance(_host_address, _port_number))
{
  std::string config_file =
      vtpl::utilities::merge_directories(vtpl::utilities::remove_last_directory(_session_folder), CONFIG_FILE);
  if (vtpl::utilities::is_regular_file_exists(config_file)) {
    _status_config = load_status_config(config_file);
  } else {
    RAY_LOG_INF << "File doesn't exist: " << config_file;
  }

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

StatusService& StatusService::getInstance(std::string session_folder, std::string host_address, uint32_t port_number)
{
  static StatusService instance(std::move(session_folder), std::move(host_address), port_number);
  return instance;
}

std::atomic_uint_fast64_t& StatusService::setStatus(int16_t app_id, int16_t channel_id, uint64_t thread_id)
{
  return _setStatusInternal(app_id, channel_id, thread_id, _status_config.process_id, _status_config.machine_id,
                            _status_config.cluster_id);
}

std::atomic_uint_fast64_t& StatusService::_setStatusInternal(int16_t app_id, int16_t channel_id, uint64_t id,
                                                             uint64_t process_id, uint64_t machine_id,
                                                             uint64_t cluster_id)
{
  size_t key = absl::HashOf(cluster_id, machine_id, process_id, app_id, channel_id, id);
  auto it = _resource_map.find(key);
  if (it != _resource_map.end()) {
    it->second->_value++;
    return it->second->_value;
  }
  std::unique_ptr<StatusAtomic> status_atomic =
      std::make_unique<StatusAtomic>(cluster_id, machine_id, process_id, app_id, channel_id, id);
  auto it_ = _resource_map.emplace(std::make_pair(key, std::move(status_atomic)));
  return it_.first->second->_value;
}

StatusConfig StatusService::load_status_config(std::string config_file)
{
  try {
    std::ifstream ifs(config_file);
    if (ifs.good()) {
      cereal::JSONInputArchive ar(ifs);
      ar(_status_config);
    }
    ifs.close();
  } catch (const std::exception& e) {
    RAY_LOG_INF << "load_status_config error: " << e.what();
  }
  return _status_config;
}

int64_t StatusService::_get_current_time_in_ms()
{
  return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
      .count();
}

void StatusService::_write_header_internal(std::shared_ptr<spdlog::logger> logger,
                                           std::map<size_t, std::unique_ptr<StatusAtomic>>& resource_map)
{
  {
    std::stringstream ss;
    for (auto&& it : resource_map) {
      ss << "app.chn.id        fps|";
    }
    write_header(logger, ss.str());
  }

  {
    std::stringstream ss;
    for (auto&& it : resource_map) {
      ss << fmt::format("{:03}.{:03}.{:03}       fps|", it.second.get()->_app_id, it.second.get()->_channel_id,
                        it.second.get()->_id);
    }
    write_log(logger, ss.str());
  }
}

void StatusService::_write_data(std::map<size_t, std::unique_ptr<StatusAtomic>>& _resource_map,
                                std::shared_ptr<spdlog::logger> logger, int64_t& last_write_time, size_t& last_map_size)
{
  if (last_map_size != _resource_map.size()) {
    _write_header_internal(logger, _resource_map);
    last_map_size = _resource_map.size();
  }

  int64_t date_timestamp = _get_current_time_in_ms();
  int64_t time_diff = date_timestamp - last_write_time;
  last_write_time = date_timestamp;
  if (time_diff <= 0) {
    return;
  }

  std::shared_ptr<resource::ClusterStatus> p_cluster_status = std::make_shared<resource::ClusterStatus>();
  resource::MachineStatus* p_machine_status;
  resource::ProcessStatus* p_process_status;
  resource::ThreadStatus* p_thread_status;

  std::stringstream ss;

  for (auto&& it : _resource_map) {
    float diff = ((static_cast<float>(it.second.get()->_value) - static_cast<float>(it.second.get()->_last_value)) *
                  1000.0F / static_cast<float>(time_diff));

    p_cluster_status->set_id(it.second.get()->_cluster_id);

    p_machine_status = p_cluster_status->add_machine_status();
    p_machine_status->set_id(it.second.get()->_machine_id);

    p_process_status = p_machine_status->add_process_status();
    p_process_status->set_id(it.second.get()->_process_id);

    p_thread_status = p_process_status->add_thread_status();
    p_thread_status->set_app_id(it.second.get()->_app_id);
    p_thread_status->set_channel_id(it.second.get()->_channel_id);
    p_thread_status->set_id(it.second.get()->_id);
    p_thread_status->set_value(it.second.get()->_value);
    p_thread_status->set_last_value(it.second.get()->_last_value);

    it.second.get()->_last_value.store(it.second.get()->_value);
    ss << fmt::format("{} {:>10.{}f}|", last_write_time / 1000, diff, 1);
  }

  write_log(logger, ss.str());
  try {
    send_to_server(p_cluster_status, time_diff, last_write_time / 1000);
  } catch (const std::exception& e) {
    RAY_LOG_INF << "send_to_server error: " << e.what();
  }
}

void StatusService::send_to_server(std::shared_ptr<::resource::ClusterStatus> p_cluster_status, int64_t time_diff,
                                   int64_t timestamp_sec)
{
  _status_service_client.set_status(std::move(p_cluster_status), time_diff, timestamp_sec);
}

void StatusService::run()
{
  int sleep_upto_sec = max_sleep_upto_sec;
  int iteration_counter = 0;
  int fps_log_counter = 0;
  auto logger = get_logger_st(_session_folder, "fps");
  size_t last_map_size = 0;
  int64_t last_write_time = _get_current_time_in_ms();

  while (!_do_shutdown) {
    if (sleep_upto_sec > 0) {
      sleep_upto_sec--;
    } else {
      sleep_upto_sec = max_sleep_upto_sec;
      _write_data(_resource_map, logger, last_write_time, last_map_size);
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  _write_data(_resource_map, logger, last_write_time, last_map_size);
}

// int main(int argc, char const* argv[])
// {
//   int i = 100000;
//   std::cout << "Started" << std::endl;
//   std::string name_of_app = "resource_monitor_test";
//   std::string a = "session/";
//   ray::RayLog::StartRayLog(name_of_app, ::ray::RayLogLevel::DEBUG, a);
//   std::atomic_uint_fast64_t& monitor_set_status =
//       StatusService::getInstance("/workspaces/monitorutil/", "127.0.0.1", 9800).setStatus(207, 1, 1);
//   while (i-- > 0) {
//     monitor_set_status++;
//     std::this_thread::sleep_for(std::chrono::milliseconds(1));
//   }
//   return 0;
// }
