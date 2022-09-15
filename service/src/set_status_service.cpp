// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include <Poco/DateTime.h>
#include <Poco/DateTimeFormat.h>
#include <Poco/DateTimeFormatter.h>
#include <absl/hash/hash.h>
#include <atomic>

#include "logging.h"
#include "set_status_service.h"

constexpr int max_sleep_upto_sec = 10;

StatusAtomic::StatusAtomic(uint64_t cluster_id, uint64_t machine_id, uint64_t process_id, uint64_t app_id,
                           uint64_t channel_id, uint64_t id, uint64_t value, uint64_t last_value)
    : _cluster_id(cluster_id), _machine_id(machine_id), _process_id(process_id), _app_id(app_id),
      _channel_id(channel_id), _id(id), _value(value), _last_value(last_value)
{
}

StatusAtomic::StatusAtomic(const StatusAtomic& o)
{
  _cluster_id = o._cluster_id.load();
  _machine_id = o._machine_id.load();
  _process_id = o._process_id.load();
  _app_id = o._app_id.load();
  _channel_id = o._channel_id.load();
  _id = o._id.load();
  _value = o._value.load();
  _last_value = o._last_value.load();
}

StatusAtomic& StatusAtomic::operator=(const StatusAtomic& o)
{
  _cluster_id = o._cluster_id.load();
  _machine_id = o._machine_id.load();
  _process_id = o._process_id.load();
  _app_id = o._app_id.load();
  _channel_id = o._channel_id.load();
  _id = o._id.load();
  _value = o._value.load();
  _last_value = o._last_value.load();
  return *this;
}

SetStatusService::SetStatusService(std::string session_folder) : _session_folder(session_folder)
{
  _thread = std::make_unique<std::thread>(&SetStatusService::run, this);
}
SetStatusService::~SetStatusService()
{
  _do_shutdown = true;
  _thread->join();
  for (auto&& it : _m_list) {
    it.second.reset();
    it.second = nullptr;
  }
}

grpc::Status SetStatusService::SetStatus(grpc::ServerContext* /*context*/, const resource::SetStatusRequest* request,
                                         resource::SetStatusResponse* response)
{
  resource::ClusterStatus cluster_status = request->cluster_status();
  _time_diff = request->time_diff();
  _timestamp_sec = request->timestamp_sec();
  for (resource::MachineStatus& machine_status : *cluster_status.mutable_machine_status()) {
    for (resource::ProcessStatus& process_status : *machine_status.mutable_process_status()) {
      for (resource::ThreadStatus& thread_status : *process_status.mutable_thread_status()) {
        size_t uuid_hash = absl::HashOf(cluster_status.id(), machine_status.id(), process_status.id(),
                                        thread_status.app_id(), thread_status.channel_id(), thread_status.id());
        StatusAtomic status_atomic(cluster_status.id(), machine_status.id(), process_status.id(),
                                   thread_status.app_id(), thread_status.channel_id(), thread_status.id(),
                                   thread_status.value(), thread_status.last_value());
        auto it = _m_list.find(uuid_hash);
        if (it != _m_list.end()) {
          *(it->second) = status_atomic;
        } else {
          _m_list.emplace(std::make_pair(uuid_hash, std::make_unique<StatusAtomic>(status_atomic)));
        }
      }
    }
  }

  response->set_error_code(0);
  return grpc::Status::OK;
}

void SetStatusService::run()
{
  int sleep_upto_sec = max_sleep_upto_sec;
  auto logger = get_logger_st(_session_folder, "fps");
  size_t last_map_size = 0;

  while (!_do_shutdown) {
    if (sleep_upto_sec > 0) {
      sleep_upto_sec--;
    } else {
      sleep_upto_sec = max_sleep_upto_sec;
      _write_data(logger, last_map_size);
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  _write_data(logger, last_map_size);
}

void SetStatusService::_write_data(std::shared_ptr<spdlog::logger> logger, size_t& last_map_size)
{
  if (last_map_size != _m_list.size()) {
    _write_header_internal(logger);
    last_map_size = _m_list.size();
  }

  std::stringstream ss;
  for (auto&& m : _m_list) {
    float diff = ((static_cast<float>(m.second->_value) - static_cast<float>(m.second->_last_value)) * 1000.0F /
                  static_cast<float>(_time_diff));
    m.second->_value = 0;
    m.second->_last_value = 0;
    ss << fmt::format("{} {:>9.{}f}|",
                      Poco::DateTimeFormatter::format(Poco::Timestamp().fromEpochTime(_timestamp_sec),
                                                      Poco::DateTimeFormat::SORTABLE_FORMAT),
                      diff, 1);
  }
  write_log(logger, ss.str());
}

void SetStatusService::_write_header_internal(std::shared_ptr<spdlog::logger> logger)
{
  {
    std::stringstream ss;
    for (auto&& it : _m_list) {
      ss << "cid.mid.pid.app.chn.id    fps|";
    }
    write_header(logger, ss.str());
  }

  {
    std::stringstream ss;
    for (auto&& it : _m_list) {
      ss << fmt::format("{:03}.{:03}.{:03}.{:03}.{:03}.{:03}   fps|", it.second.get()->_cluster_id,
                        it.second.get()->_machine_id, it.second.get()->_process_id, it.second.get()->_app_id,
                        it.second.get()->_channel_id, it.second.get()->_id);
    }
    write_log(logger, ss.str());
  }
}