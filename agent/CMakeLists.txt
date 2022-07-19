# *****************************************************
#    Copyright 2022 Videonetics Technology Pvt Ltd
# *****************************************************

find_package(logutil REQUIRED)

add_library(status_service_agent
    src/status_service.cpp

    src/status_service_grpc_caller.cpp
    src/status_service_client.cpp
)

target_include_directories(status_service_agent
    PRIVATE inc
    PUBLIC include
)

target_link_libraries(status_service_agent
    PUBLIC protobuf_helper
    PRIVATE logutil::core
)

target_link_libraries(status_service_agent
    PUBLIC grpc_helper
)