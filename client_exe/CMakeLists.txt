# *****************************************************
#    Copyright 2022 Videonetics Technology Pvt Ltd
# *****************************************************

find_package(absl REQUIRED)
find_package(logutil REQUIRED)
find_package(Poco REQUIRED Util)

add_executable(client_exe
    src/entry_point.cpp
    src/globalerrorhandler.cpp
)

target_include_directories(client_exe
    PRIVATE inc
    PRIVATE ${PROJECT_BINARY_DIR}
)

target_link_libraries(client_exe
    PRIVATE Poco::Util
    PRIVATE logutil::core
    PRIVATE absl::strings
    PRIVATE status_service_agent
    PUBLIC utilities
)

target_compile_features(client_exe PRIVATE cxx_std_17)