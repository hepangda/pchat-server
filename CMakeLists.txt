cmake_minimum_required(VERSION 3.0)
project(pchat-server)

set(CMAKE_CXX_STANDARD 20)
link_libraries(pthread)
add_definitions("-O3")

include_directories(.)
include_directories(source/std-ext)

aux_source_directory(source/protocol SOURCE_PROTOS)

find_package(fmt CONFIG REQUIRED)
link_libraries(fmt::fmt)

find_package(ZeroMQ CONFIG REQUIRED)
link_libraries(libzmq libzmq-static)

find_package(cppzmq CONFIG REQUIRED)
link_libraries(cppzmq cppzmq-static)

find_package(libmysql REQUIRED)
link_libraries(${MYSQL_LIBRARIES})

find_package(SOCI CONFIG REQUIRED)
link_libraries(SOCI::soci_mysql_static SOCI::soci_core_static SOCI::soci_empty_static)

find_package(gflags CONFIG REQUIRED)
link_libraries(gflags::gflags)

find_package(Protobuf REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS})
link_libraries(protobuf::libprotoc protobuf::libprotobuf protobuf::libprotobuf-lite)

execute_process(COMMAND ${PROJECT_SOURCE_DIR}/source/protocol/proto/generate_proto.sh ${PROTOBUF_PROTOC_EXECUTABLE}
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/source/protocol/proto)

add_subdirectory(3rdparty/json2pb)
link_libraries(json2pb)

add_executable(pchat-server
        pchat-server.cpp

        ${SOURCE_PROTOS}

        source/core/concurrency_manager.cc
        source/core/concurrency_manager.h
        source/core/context.cc
        source/core/context.h
        source/core/network_manager.cc
        source/core/network_manager.h
        source/core/pchat_server.cc
        source/core/pchat_server.h
        source/core/persistence_manager.cc
        source/core/persistence_manager.h
        source/core/watchdog.cc
        source/core/watchdog.h

        source/foundation/foundation.h

        source/service/service.h
        source/service/service.cc
        source/service/account_service.cc
        source/service/account_service.h
        source/service/group_service.cc
        source/service/group_service.h
        source/service/service_manager.cc
        source/service/service_manager.h

        source/utils/arguments_manager.cc
        source/utils/arguments_manager.h
        source/utils/file.cc
        source/utils/file.h

        source/network/net_worker.cc
        source/network/net_worker.h
        )

add_executable(protoc-gen-pchat
        ${SOURCE_PROTOS}
        tools/protoc-gen-pchat.cc)

add_executable(pchat-test-tools
        ${SOURCE_PROTOS}
        test/test_tools.cc)

add_custom_command(TARGET pchat-server POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/pchat.json $<TARGET_FILE_DIR:pchat-server>)

