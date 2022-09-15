#include <cereal/archives/json.hpp>
#include <cereal/cereal.hpp>

struct StatusConfig {
  int cluster_id;
  int machine_id;
  int process_id;
  StatusConfig() = default;
  ~StatusConfig() = default;
  template <class Archive> void save(Archive& ar) const
  {
    ar(cereal::make_nvp("ClusterId", cluster_id));
    ar(cereal::make_nvp("MachineId", machine_id));
    ar(cereal::make_nvp("ProcessId", process_id));
  }

  template <class Archive> void load(Archive& ar)
  {
    ar(cereal::make_nvp("ClusterId", cluster_id));
    ar(cereal::make_nvp("MachineId", machine_id));
    ar(cereal::make_nvp("ProcessId", process_id));
  }
};

void epilogue(cereal::JSONInputArchive& /*unused*/, const StatusConfig& /*unused*/);
void prologue(cereal::JSONInputArchive& /*unused*/, const StatusConfig& /*unused*/);
void epilogue(cereal::JSONOutputArchive& /*unused*/, const StatusConfig& /*unused*/);
void prologue(cereal::JSONOutputArchive& /*unused*/, const StatusConfig& /*unused*/);