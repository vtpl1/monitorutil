# *****************************************************
#    Copyright 2022 Videonetics Technology Pvt Ltd
# *****************************************************

find_package(absl REQUIRED)
find_package(logutil REQUIRED)
find_package(Poco REQUIRED Util)

add_executable(service_exe
    src/entry_point.cpp
    src/globalerrorhandler.cpp
    src/utilities.cpp
)

target_include_directories(service_exe
    PRIVATE inc
    PRIVATE ${PROJECT_BINARY_DIR}
)

target_link_libraries(service_exe
    PRIVATE Poco::Util
    PRIVATE logutil::core
    PRIVATE absl::strings
)

target_compile_features(service_exe PRIVATE cxx_std_17)