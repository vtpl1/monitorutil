// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************
#include <iomanip>
#include <logging.h>
#include <sstream>

#include "monitor.h"

constexpr int MONITOR_PORT_NUMBER = 29000;
constexpr int max_sleep_upto_sec = 10;

Monitor::Monitor(std::string session_folder, std::string target_host_address, uint16_t target_port)
    : _session_folder(std::move(session_folder)), _target_host_address(std::move(target_host_address)),
      _target_port(target_port)
{
  _thread = std::make_unique<std::thread>(&Monitor::run, this);
}

Monitor& Monitor::getInstance(std::string session_folder, std::string target_host_address, uint16_t target_port)
{
  static Monitor instance(std::move(session_folder), std::move(target_host_address), target_port);
  return instance;
}

Monitor& Monitor::getInstance() { return getInstance("session"); }

Monitor& Monitor::getInstance(std::string session_folder)
{
  return getInstance(std::move(session_folder), "127.0.0.1", MONITOR_PORT_NUMBER);
}

Monitor::~Monitor()
{
  _do_shutdown = true;
  _thread->join();
  for (auto&& it : _resource_map) {
    it.second.reset();
    it.second = nullptr;
  }
}

std::string getCurrentTimeStr()
{
  std::chrono::system_clock::time_point tp = std::chrono::system_clock::now();
  std::stringstream ss;
  auto t = std::chrono::system_clock::to_time_t(tp);
  // auto tp2 = std::chrono::system_clock::from_time_t(t);
  // if (tp2 > tp) {
  //   t = std::chrono::system_clock::to_time_t(tp - std::chrono::seconds(1));
  // }
  // ss << std::put_time(std::gmtime(&t), "%Y-%m-%d %H:%M:%S ") << "." << std::setfill('0') << std::setw(3)
  //    << (std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch()).count() % 1000);
  ss << std::put_time(std::gmtime(&t), "%F %T ") << "." << std::setfill('0') << std::setw(3)
     << (std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch()).count() % 1000);
  return ss.str();
  // Poco::Timestamp now;
  // return Poco::DateTimeFormatter::format(now, Poco::DateTimeFormat::SORTABLE_FORMAT);
}

int64_t getCurrentTimeInMs()
{
  // std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
  // auto duration = now.time_since_epoch();
  // auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
  return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
      .count();
  // return millis;
}
std::atomic_uint_fast64_t& Monitor::setStatusInternal(uint64_t id)
{
  // if (_is_already_shutting_down) {
  //   return;
  // }
  auto it = _resource_map.find(id);
  if (it != _resource_map.end()) {
    it->second->value++;
    // it->second->last_update_time_in_ms.store(getCurrentTimeInMs());
    return it->second->value;
  }
  auto it1 = _resource_map.emplace(std::make_pair(id, std::move(std::make_unique<Status>())));
  return it1.first->second->value;
}
uint64_t get_unique_id_from_app_id_channel_id_id(int16_t app_id, int16_t channel_id, uint64_t id)
{
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  return app_id * 100000 + channel_id * 100 + id;
}

std::tuple<int16_t, int16_t, uint64_t> get_app_id_channel_id_id_from_unique_id(uint64_t unique_id)
{
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  auto app_id = static_cast<int16_t>(unique_id / 100000);
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  auto channel_id_id = static_cast<int16_t>(unique_id % 100000);
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  auto channel_id = channel_id_id / 100;
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  uint64_t id = channel_id_id % 100;
  return {app_id, channel_id, id};
}

void write_header_internal(std::shared_ptr<spdlog::logger> logger,
                           std::map<uint64_t, std::unique_ptr<Status>>& resource_map)
{
  {
    std::stringstream ss;
    for (auto&& it : resource_map) {
      ss << "app.chn. id   fps|";
    }
    write_header(logger, ss.str());
    write_log(logger, getCurrentTimeStr());
  }

  {
    std::stringstream ss;
    for (auto&& it : resource_map) {
      std::tuple<int16_t, int16_t, uint64_t> a_c_i = get_app_id_channel_id_id_from_unique_id(it.first);
      ss << fmt::format("{:03}.{:03}.{:03}   fps|", std::get<0>(a_c_i), std::get<1>(a_c_i), std::get<2>(a_c_i));
    }
    write_log(logger, ss.str());
  }
}

void writeData(std::map<uint64_t, std::unique_ptr<Status>>& _resource_map, std::shared_ptr<spdlog::logger> logger,
               int64_t& last_write_time, size_t& last_map_size)
{
  ::resource::MachineStatus machine_status;
  machine_status.set_id(1);
  machine_status.set_channel_id(1);
  ::resource::ProcessStatus* p_process_status = machine_status.add_process_status();
  p_process_status->set_id(1);
  p_process_status->set_channel_id(1);
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

    ::resource::ThreadStatus* l_p_th = p_process_status->add_thread_status();
    l_p_th->set_id(it.first);
    l_p_th->set_channel_id(it.first);
    l_p_th->set_value(it.second->value);
    l_p_th->set_last_value(it.second->last_value);
    // l_p_th->set_last_updated_in_ms(it.second->last_update_time_in_ms);
    it.second->last_value.store(it.second->value);

    std::tuple<int16_t, int16_t, uint64_t> a_c_i = get_app_id_channel_id_id_from_unique_id(it.first);
    {

      // ss << std::setfill('0') << std::setw(3) << std::get<0>(a_c_i) << "." << std::setfill('0') << std::setw(3)
      //    << std::get<1>(a_c_i) << "." << std::setfill('0') << std::setw(2) << std::get<2>(a_c_i) << " : "
      //    << std::setfill('0') << std::setw(5) << std::fixed << std::setprecision(2) << diff << "   ";
      ss << fmt::format("{:>17.{}f}|", diff, 1);
    }
  }
  write_log(logger, ss.str());
  // send_to_server(s, machine_status, date_timestamp, _target_host_address, _target_port);
}
void Monitor::run()
{
  int sleep_upto_sec = max_sleep_upto_sec;
  int iteration_counter = 0;
  int fps_log_counter = 0;
  auto logger = get_logger_st(_session_folder, "fps");
  size_t last_map_size = 0;
  int64_t last_write_time = getCurrentTimeInMs();
  // std::shared_ptr<Poco::Net::StreamSocket> s;
  while (!_do_shutdown) {
    if (sleep_upto_sec > 0) {
      sleep_upto_sec--;
    } else {
      sleep_upto_sec = max_sleep_upto_sec;
      writeData(_resource_map, logger, last_write_time, last_map_size);
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  writeData(_resource_map, logger, last_write_time, last_map_size);
}

std::atomic_uint_fast64_t& Monitor::setStatus(int16_t app_id, int16_t channel_id, uint64_t id)
{
  return Monitor::getInstance().setStatusInternal(get_unique_id_from_app_id_channel_id_id(app_id, channel_id, id));
}
