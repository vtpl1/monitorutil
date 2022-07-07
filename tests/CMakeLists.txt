find_package(Catch2 REQUIRED)

add_executable(tests
    test_status_service.cpp
)

target_link_libraries(tests
    PRIVATE Catch2::Catch2WithMain
    PRIVATE status_service_agent
    PRIVATE logutil::core
)