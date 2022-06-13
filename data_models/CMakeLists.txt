# *****************************************************
#    Copyright 2022 Videonetics Technology Pvt Ltd
# *****************************************************
find_package(Protobuf COMPONENTS libprotobuf protoc)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS interfaces/status.proto)

add_library(protobuf_helper_resource_monitor STATIC
    src/protobuf_helper.cpp

    ${PROTO_SRCS}
)

target_precompile_headers(protobuf_helper_resource_monitor
  PUBLIC ${PROTO_HDRS}
  PUBLIC inc/protobuf_helper.h
)

target_include_directories(protobuf_helper_resource_monitor
    PUBLIC inc
    PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(protobuf_helper_resource_monitor
    PUBLIC protobuf::libprotobuf
)

set_target_properties(
    protobuf_helper_resource_monitor
    PROPERTIES
         CXX_CLANG_TIDY ""
)