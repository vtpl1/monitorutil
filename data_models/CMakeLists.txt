# *****************************************************
#    Copyright 2022 Videonetics Technology Pvt Ltd
# *****************************************************
find_package(Protobuf COMPONENTS libprotobuf protoc)
find_package(cereal REQUIRED)

set(PROTOBUF_HELPER_PROTO_FILES
    interfaces/status.proto
)

# protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS interfaces/status.proto)

add_library(protobuf_helper STATIC
    src/protobuf_helper.cpp

    # ${PROTO_SRCS}
)

protobuf_generate(TARGET protobuf_helper
    PROTOS ${PROTOBUF_HELPER_PROTO_FILES}
    LANGUAGE cpp
    GENERATE_EXTENSIONS .pb.h .pb.cc
)

target_precompile_headers(protobuf_helper
#   PUBLIC ${PROTO_HDRS}
    PUBLIC inc/protobuf_helper.h
)

target_include_directories(protobuf_helper
    PUBLIC inc
    PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(protobuf_helper
    PUBLIC protobuf::libprotobuf
)

set_target_properties(
    protobuf_helper
    PROPERTIES
         CXX_CLANG_TIDY ""
         C_CLANG_TIDY ""
)

add_library(data_models
    src/data_models.cpp
)

target_include_directories(data_models
    PUBLIC inc
)

target_link_libraries(data_models
    PUBLIC cereal::cereal
)


if (TRUE)
    find_package(gRPC REQUIRED) # use protobuf implicitly
    #
    # Compile protobuf and grpc files in myproto target to cpp
    #
    get_target_property(grpc_cpp_plugin_location gRPC::grpc_cpp_plugin LOCATION)



    set(GRPC_HELPER_PROTO_FILES
        interfaces/statusservice.proto
    )

    add_library(grpc_helper STATIC
        src/grpc_helper.cpp
    )

    target_include_directories(grpc_helper
        PUBLIC inc
        PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
    )

    target_link_libraries(grpc_helper
        PUBLIC gRPC::grpc
        PUBLIC gRPC::grpc++
        PUBLIC protobuf_helper
    )

    protobuf_generate(TARGET grpc_helper
        PROTOS ${GRPC_HELPER_PROTO_FILES}
        LANGUAGE cpp
        GENERATE_EXTENSIONS .pb.h .pb.cc
    )

    protobuf_generate(TARGET grpc_helper
        PROTOS ${GRPC_HELPER_PROTO_FILES}
        LANGUAGE grpc
        GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc
        PLUGIN protoc-gen-grpc=${grpc_cpp_plugin_location}
    )

    target_precompile_headers(grpc_helper
        PUBLIC inc/grpc_helper.h
    )

    set_target_properties(
        grpc_helper
        PROPERTIES
            CXX_CLANG_TIDY ""
            C_CLANG_TIDY ""
    )
endif ()