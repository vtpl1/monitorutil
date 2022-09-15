// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef other_utilities_h
#define other_utilities_h
#include <map>
#include <string>

namespace vtpl
{
namespace utilities
{
void get_publishable_file_list(std::map<int64_t, std::string>& list,
                               std::map<std::string, size_t>& existing_file_size_map, const std::string& event_img_dir);
std::vector<std::string> get_parsed_command(const std::string& path);
} // namespace utilities
} // namespace vtpl

#endif // other_utilities_h
