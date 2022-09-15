# *****************************************************
#    Copyright 2022 Videonetics Technology Pvt Ltd
# *****************************************************

cmake_minimum_required (VERSION 3.20)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(Poco REQUIRED Foundation Util XML)
find_package(absl REQUIRED)
# find_package(tinyxml REQUIRED)

add_library(utilities
    src/utilities.cpp
    # src/utilities1.cpp
    src/sys_timestamp.cpp
    src/other_utilities.cpp
)

target_include_directories(utilities
    PUBLIC inc
)

target_link_libraries(utilities
    PRIVATE absl::strings
    PRIVATE data_models
    PRIVATE Poco::Foundation
    PRIVATE Poco::Util
    PRIVATE Poco::XML
    PRIVATE logutil::core
    # PRIVATE aesencr
    # PRIVATE unofficial-tinyxml::unofficial-tinyxml
)



if (MSVC)
else()
target_link_libraries(utilities
	PUBLIC stdc++fs
)
endif (MSVC)

# set_property(TARGET utilities PROPERTY
#   MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
