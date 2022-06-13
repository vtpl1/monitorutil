# *****************************************************
#    Copyright 2022 Videonetics Technology Pvt Ltd
# *****************************************************

add_library(monitor_agent
    src/monitor.cpp

    src/monitor_service_grpc_caller.cpp
    src/monitor_service_client.cpp
)

target_include_directories(monitor_agent
    PRIVATE inc
    PUBLIC include
)

target_link_libraries(monitor_agent
    PUBLIC protobuf_helper_monitor
    PRIVATE logutil::core
)

target_link_libraries(monitor_agent
    PUBLIC protobuf_grpc_helper_monitor
)

add_executable(monitor_agent_test
    src/monitor_test.cpp
)

target_link_libraries(monitor_agent_test
    PUBLIC monitor_agent
    PRIVATE logutil::core
)
