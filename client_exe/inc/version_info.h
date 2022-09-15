#pragma once
#ifndef version_info_h
#define version_info_h

#include <iostream>
#include <string>

namespace vtpl
{
  namespace version_info
  {
    std::string get(const std::string& app_name, const std::string& version_major,
                         const std::string& version_minor, const std::string& build_number);

  } // namespace version_info
} // namespace vtpl

#endif // !version_info_h
