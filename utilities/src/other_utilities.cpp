// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************
#include <absl/strings/str_replace.h>
#include <absl/strings/str_split.h>
#include <fstream>

#include "other_utilities.h"
#include "utilities.h"

void vtpl::utilities::get_publishable_file_list(std::map<int64_t, std::string>& list,
                                                std::map<std::string, size_t>& existing_file_size_map,
                                                const std::string& event_img_dir)
{
  std::map<int64_t, std::string> temp;
  vtpl::utilities::list_files(event_img_dir, temp, "jpg");
  if (temp.empty()) {
    vtpl::utilities::list_files(event_img_dir, temp, "jpeg");
  }

  for (auto&& i : temp) {
    size_t current_file_size = vtpl::utilities::get_file_size_bytes(i.second);
    auto it = existing_file_size_map.find(i.second);
    if (it == existing_file_size_map.end()) {
      existing_file_size_map.emplace(std::pair<std::string, size_t>(i.second, current_file_size));
    } else {
      if (it->second == current_file_size) {
        list.emplace(i);
        existing_file_size_map.erase(it->first);
      } else {
        it->second = current_file_size;
      }
    }
  }
}

std::vector<std::string> vtpl::utilities::get_parsed_command(const std::string& path)
{
  std::string str;
  std::ifstream ifs(path, std::ios::in);
  if (!ifs.good()) {
    return std::vector<std::string>();
  }
  getline(ifs, str);
  std::vector<std::string> v = absl::StrSplit(str, ' ', absl::SkipWhitespace());
  for (std::string& i : v) {
    absl::StrReplaceAll({{"./", ""}, {".\\", ""}}, &i);
#ifdef _WIN32
    absl::StrReplaceAll({{"/", vtpl::utilities::get_filesystem_directory_seperator()}}, &i);
#else
    absl::StrReplaceAll({{"\\", vtpl::utilities::get_filesystem_directory_seperator()}}, &i);
#endif
  }

  // std::for_each(v.begin(), v.end(), [](std::string &s){ vtpl::utilities::trim(s) });

  return v;
}