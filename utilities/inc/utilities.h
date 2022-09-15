// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef utilities_h
#define utilities_h

#include <map>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

namespace vtpl
{
namespace utilities
{
std::string get_filesystem_directory_seperator();
bool is_directory_exists(const std::string& dir_path);
bool is_regular_file_exists(const std::string& file_path);
bool create_directories(const std::string& dir_path);
std::string get_directories_from_file_path(const std::string& file_path);
std::string create_directories_from_file_path(const std::string& file_path);
std::stringstream end_with_directory_seperator(const std::string& dir_path_0);
std::string merge_directories(const std::string& dir_path_0, const std::string& dir_path_1);
std::string merge_directories(const std::string& dir_path_0, const std::string& dir_path_1,
                              const std::string& dir_path_2);
std::string merge_directories(const std::string& dir_path_0, const std::string& dir_path_1,
                              const std::string& dir_path_2, const std::string& dir_path_3);
std::string merge_directories(const std::string& dir_path_0, const std::string& dir_path_1,
                              const std::string& dir_path_2, const std::string& dir_path_3,
                              const std::string& dir_path_4);
std::string remove_last_directory(const std::string& dir_path1);
std::string merge_directories(const std::string& dir_path, std::vector<std::string>& dir_n_list);
bool delete_directory(const std::string& dir_path);
bool delete_file(const std::string& file_path);
std::string base64_encode_file(const std::string& file_path);
std::string generate_unique_id();
int64_t get_file_end_timestamp(const std::string& path);
bool get_file_start_timestamp(std::string file_name, int64_t& start_timestamp, int64_t& end_timestamp);
int64_t get_last_modified_time(const std::string& file_abs_path);
void list_files(const std::string& path, std::map<int64_t, std::string>& list, const std::string& extension);
void list_files_sorted(const std::string& path, std::map<int64_t, std::string>& list, const std::string& extension);
void glob_files(const std::string& path, std::vector<std::string>& list, const std::string& file_name);
size_t get_file_size_bytes(const std::string& abs_file_path);
std::string get_file_name(const std::string& file_name);
std::string shorten_string(const std::string& str);
std::string change_extension(const std::string& str, const std::string& ext);
std::string rename_file(const std::string& old_file, const std::string& new_name);
std::string compose_url(const std::string& url, const std::string& user, const std::string& pass);
std::string get_environment_value(const std::string& value, const std::string& default_value);
void from_file_to_vector(const std::string& file_path, std::vector<int>& vec);
void from_file_to_vector(const std::string& file_path, std::vector<std::string>& vec);
void from_file_to_map(const std::string& file_path, std::map<std::string, int>& map);
int get_usable_number(const std::string& key, int start, int end, const std::string& file_path);
std::tuple<int64_t, int64_t, int> get_capacity_available_percentage_space_info(const std::string& dir_path);
std::string get_first_two_letters_in_upper_case(const std::string& str);
// std::string form_write_path(std::string base_path, std::string customer_id, std::string device_unique_id,
//                             vtpl::file_type* p_file_type, vtpl::general_stream_type* p_stream_type,
//                             long long timestamp);
bool copy_file(const std::string& src_path, const std::string& dst_dir);
std::string get_absolute_path(const std::string& path);
int64_t get_current_time_in_millis();
std::tuple<std::string, std::string> parse_user_pass(const std::string& url);
std::string parse_ip_from_url(const std::string& url);
std::string encrypt(const std::string& payLoad, const std::string& passPhrase);
std::string decrypt(const std::string& payLoad, const std::string& passPhrase);
} // namespace utilities
} // namespace vtpl

#endif // utilities_h
