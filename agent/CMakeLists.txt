# *****************************************************
#    Copyright 2022 Videonetics Technology Pvt Ltd
# *****************************************************

find_package(logutil REQUIRED)

add_library(status_service_agent
    src/status_service.cpp
)

target_include_directories(status_service_agent
    PUBLIC include
)

target_link_libraries(status_service_agent
    PRIVATE logutil::core
)
