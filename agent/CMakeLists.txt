# *****************************************************
#    Copyright 2022 Videonetics Technology Pvt Ltd
# *****************************************************

add_library(resource_monitor
    src/monitor.cpp
)

target_include_directories(resource_monitor
    PRIVATE inc
    PUBLIC include
)

target_link_libraries(resource_monitor
    PUBLIC protobuf_helper_resource_monitor
    PRIVATE logutil::core
)

add_executable(resource_monitor_test
    src/monitor_test.cpp
)

target_link_libraries(resource_monitor_test
    PUBLIC resource_monitor
    PRIVATE logutil::core
)