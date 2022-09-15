#include <Poco/NumberParser.h>
#include <Poco/Util/HelpFormatter.h>
#include <Poco/Util/ServerApplication.h>
#include <Poco/Util/XMLConfiguration.h>
#include <absl/strings/str_split.h>
#include <iostream>
#include <sstream>
#include <string_view>

// #include "Application.h"
#include "globalerrorhandler.h"
#include "logging.h"
#include "status_service.h"
#include "utilities.h"
#include "version.h"
#include "version_info.h"

const std::string SESSION_FOLDER = "session";

class entry_point : public Poco::Util::ServerApplication
{
private:
  std::string_view CONFIGURATION_FILE_NAME{"conf.xml"};
  std::string_view BASE_LOG_DIR_CONFIGURATION_NAME{"logDir"};
  std::string_view BASE_DATA_DIR_CONFIGURATION_NAME{"dataDir"};
  std::string_view BASE_CONFIG_DIR_CONFIGURATION_NAME{"configDir"};
  std::string_view BASE_LOG_LEVEL_CONFIGURATION_NAME{"logLevel"};

  std::string_view LOG_DIR_NAME{"Logs"};
  std::string_view DATA_DIR_NAME{"Data"};
  std::string_view CONFIG_DIR_NAME{"Config"};

  std::string_view DATA_DIR_ENV_VARIABLE_NAME{"DATA_ROOT"};

  std::string _name_of_app{};
  vtpl::globalerrorhandler _globalerrorhandler;

  std::string _application_base_path;
  std::string _base_log_directory_path;
  std::string _base_data_directory_path;
  std::string _base_config_directory_path;

  Poco::AutoPtr<Poco::Util::XMLConfiguration> _p_first_conf;
  Poco::AutoPtr<Poco::Util::XMLConfiguration> _p_conf;

  std::string _host_address{"127.0.0.1"};
  uint32_t _port_number{9800};
  uint16_t _fps{25};

  std::atomic_bool _do_shutdown{false};

  bool _start_playing{true};

protected:
  std::string get_application_running_folder()
  {
    Poco::Path base_path;
    if (config().getBool("application.runAsService", false) || config().getBool("application.runAsDaemon", false)) {
      base_path.assign(config().getString("application.dataDir", Poco::Path::current()));
      base_path.pushDirectory(config().getString("application.baseName", "ojana"));
    } else {
      base_path.assign(Poco::Path::current());
    }
    return base_path.toString();
  }

  std::string get_session_folder()
  {
    Poco::Path base_path(get_application_running_folder());
    base_path.pushDirectory(SESSION_FOLDER);
    return base_path.toString();
  }

  std::string get_application_installation_folder()
  {
    Poco::Path base_path;
    if (config().getBool("application.runAsService", false) || config().getBool("application.runAsDaemon", false)) {
      base_path.assign(
          config().getString("application.dataDir", config().getString("application.dir", Poco::Path::current())));
      base_path.pushDirectory(config().getString("application.baseName", "ojana"));
    } else {
      base_path.assign(config().getString("application.dir", Poco::Path::current()));
    }
    return base_path.toString();
  }

  void initialize(Application& self) override
  {
    loadConfiguration();
    ServerApplication::initialize(self);
    Poco::ErrorHandler::set(&_globalerrorhandler);
    load_first_configuration();
    load_configuration();
    _name_of_app = config().getString("application.baseName");
    _host_address = config().getString("host_address");
    _port_number = config().getUInt("port_number");
    _fps = config().getUInt("fps");
    std::string s = vtpl::utilities::end_with_directory_seperator(_base_log_directory_path).str();
    ::ray::RayLog::StartRayLog(_name_of_app, ::ray::RayLogLevel::INFO, s, false);
  }

  void defineOptions(Poco::Util::OptionSet& options) override
  {
    options.addOption(Poco::Util::Option("host_address", "h", "host address")
                          .required(false)
                          .repeatable(false)
                          .argument("value")
                          .binding("host_address"));
    options.addOption(Poco::Util::Option("port_number", "p", "port number")
                          .required(false)
                          .repeatable(false)
                          .argument("value")
                          .binding("port_number"));
    options.addOption(Poco::Util::Option("fps", "f", "frame per second")
                          .required(false)
                          .repeatable(false)
                          .argument("value")
                          .binding("fps"));
  }

  void handleOption(const std::string& name, const std::string& value) override
  {
    std::cout << "options: name : [" << name << "] value: [" << value << "]" << std::endl;
    ServerApplication::handleOption(name, value);
  }

  static void display_info()
  {
    // std::cout << vtpl::version_info::get(APPLICATION_NAME, std::to_string(VERSION_MAJOR),
    // std::to_string(VERSION_MINOR),
    //                                      BUILD_NUMBER)
    //           << std::endl;
  }

public:
  entry_point() : _p_first_conf(new Poco::Util::XMLConfiguration()), _p_conf(new Poco::Util::XMLConfiguration())
  {
    setUnixOptions(true);
  }

  // void get_application_base_dirs()
  // {
  // _base_config_directory_path = config().getString("application.configDir");
  // _base_log_directory_path = config().getString("application.configDir");
  // _base_data_directory_path = config().getString("application.dataDir");
  // }

  void load_first_configuration()
  {
    int config_file_save_counter = 0;

    _application_base_path = get_session_folder();

    // config_file configuration
    std::string first_conf_file_path = vtpl::utilities::merge_directories(
        _application_base_path, std::string(CONFIG_DIR_NAME), std::string(CONFIGURATION_FILE_NAME));
    if (vtpl::utilities::is_regular_file_exists(first_conf_file_path)) {
      _p_first_conf->load(first_conf_file_path);
    } else {
      config_file_save_counter++;
    }

    // base_config_dir configuration
    _base_config_directory_path =
        vtpl::utilities::merge_directories(_application_base_path, std::string(CONFIG_DIR_NAME));

    if (!_p_first_conf->has(std::string(BASE_CONFIG_DIR_CONFIGURATION_NAME))) {
      _p_first_conf->setString(std::string(BASE_CONFIG_DIR_CONFIGURATION_NAME), _base_config_directory_path);
      config_file_save_counter++;
    }
    _base_config_directory_path =
        _p_first_conf->getString(std::string(BASE_CONFIG_DIR_CONFIGURATION_NAME), _base_config_directory_path);

    vtpl::utilities::create_directories(_base_config_directory_path);
    if (config_file_save_counter > 0) {
      _p_first_conf->save(first_conf_file_path);
    }
  }

  void load_configuration()
  {
    int config_file_save_counter = 0;

    // config_file configuration
    std::string conf_file_path =
        vtpl::utilities::merge_directories(_base_config_directory_path, std::string(CONFIGURATION_FILE_NAME));
    if (vtpl::utilities::is_regular_file_exists(conf_file_path)) {
      _p_first_conf->load(conf_file_path);
    } else {
      config_file_save_counter++;
    }

    // base_log_dir configuration
    _base_log_directory_path = vtpl::utilities::merge_directories(_application_base_path, std::string(LOG_DIR_NAME));

    if (!_p_conf->has(std::string{BASE_LOG_DIR_CONFIGURATION_NAME})) {
      _p_conf->setString(std::string(BASE_LOG_DIR_CONFIGURATION_NAME), _base_log_directory_path);
      config_file_save_counter++;
    }
    _base_log_directory_path =
        _p_conf->getString(std::string(BASE_LOG_DIR_CONFIGURATION_NAME), _base_log_directory_path);

    vtpl::utilities::create_directories(_base_log_directory_path);

    // base data dir configuration
    _base_data_directory_path = vtpl::utilities::merge_directories(_application_base_path, std::string(DATA_DIR_NAME));

    if (!_p_conf->has(std::string{BASE_DATA_DIR_CONFIGURATION_NAME})) {
      _p_conf->setString(std::string(BASE_DATA_DIR_CONFIGURATION_NAME), _base_data_directory_path);
      config_file_save_counter++;
    }
    _base_data_directory_path =
        _p_conf->getString(std::string(BASE_DATA_DIR_CONFIGURATION_NAME), _base_data_directory_path);

    vtpl::utilities::create_directories(_base_data_directory_path);

    if (config_file_save_counter > 0) {
      _p_conf->save(conf_file_path);
    }
  }

  void thread_run(std::atomic_uint_fast64_t& monitor_set_status)
  {
    while (!_do_shutdown) {
      monitor_set_status++;
      std::this_thread::sleep_for(std::chrono::milliseconds(1000 / _fps));
    }
  }

  int main(const ArgVec& /*args*/) final
  {
    RAY_LOG_INF << "Started";
    std::atomic_uint_fast64_t& monitor_set_status =
        StatusService::getInstance(get_session_folder(), _host_address, _port_number).setStatus(207, 1, 1);
    std::thread _thread = std::thread(&entry_point::thread_run, this, std::ref(monitor_set_status));
    waitForTerminationRequest();
    _do_shutdown = true;
    if (_thread.joinable()) {
      _thread.join();
    }
    RAY_LOG_INF << "Stopped";
    return Application::EXIT_OK;
  }
};
POCO_SERVER_MAIN(entry_point);