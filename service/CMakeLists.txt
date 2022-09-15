# *****************************************************
#    Copyright 2022 Videonetics Technology Pvt Ltd
# *****************************************************

find_package(gRPC REQUIRED)
find_package(Poco REQUIRED Util)
find_package(logutil REQUIRED)

add_library(set_status_service
    src/set_status_service.cpp
    src/set_status_service_run.cpp
)

target_include_directories(set_status_service
    PUBLIC inc
)

target_link_libraries(set_status_service
    PRIVATE gRPC::grpc++
    PRIVATE logutil::core
    PUBLIC grpc_helper
    PRIVATE Poco::Util
    PRIVATE utilities
)