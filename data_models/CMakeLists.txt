# *****************************************************
#    Copyright 2022 Videonetics Technology Pvt Ltd
# *****************************************************
find_package(Protobuf COMPONENTS libprotobuf protoc)

set(PROTOBUF_HELPER_MONITOR_PROTO_FILES
    interfaces/status.proto
)

# protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS interfaces/status.proto)

add_library(protobuf_helper_monitor STATIC
    src/protobuf_helper.cpp

    # ${PROTO_SRCS}
)

protobuf_generate(TARGET protobuf_helper_monitor
    PROTOS ${PROTOBUF_HELPER_MONITOR_PROTO_FILES}
    LANGUAGE cpp
    GENERATE_EXTENSIONS .pb.h .pb.cc
)

target_precompile_headers(protobuf_helper_monitor
#   PUBLIC ${PROTO_HDRS}
  PUBLIC inc/protobuf_helper.h
)

target_include_directories(protobuf_helper_monitor
    PUBLIC inc
    PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(protobuf_helper_monitor
    PUBLIC protobuf::libprotobuf
)

set_target_properties(
    protobuf_helper_monitor
    PROPERTIES
         CXX_CLANG_TIDY ""
)


if (TRUE)
    find_package(gRPC REQUIRED) # use protobuf implicitly
    #
    # Compile protobuf and grpc files in myproto target to cpp
    #
    get_target_property(grpc_cpp_plugin_location gRPC::grpc_cpp_plugin LOCATION)



    set(PROTOBUF_GRPC_HELPER_MONITOR_PROTO_FILES
        interfaces/monitorservice.proto
    )

    add_library(protobuf_grpc_helper_monitor STATIC
        src/protobuf_grpc_helper.cpp
    )

    target_include_directories(protobuf_grpc_helper_monitor
        PUBLIC inc
        PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
    )

    message(STATUS "MONOTOSH: " ${CMAKE_CURRENT_BINARY_DIR})

    target_link_libraries(protobuf_grpc_helper_monitor
        PUBLIC gRPC::grpc
        PUBLIC gRPC::grpc++
        PUBLIC protobuf_helper_monitor
    )

    protobuf_generate(TARGET protobuf_grpc_helper_monitor
        PROTOS ${PROTOBUF_GRPC_HELPER_MONITOR_PROTO_FILES}
        LANGUAGE cpp
        GENERATE_EXTENSIONS .pb.h .pb.cc
    )

    protobuf_generate(TARGET protobuf_grpc_helper_monitor
        PROTOS ${PROTOBUF_GRPC_HELPER_MONITOR_PROTO_FILES}
        LANGUAGE grpc
        GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc
        PLUGIN protoc-gen-grpc=${grpc_cpp_plugin_location}
    )

    target_precompile_headers(protobuf_grpc_helper_monitor
        PUBLIC inc/protobuf_grpc_helper.h
    )

endif ()