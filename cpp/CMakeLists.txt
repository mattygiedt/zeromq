cmake_minimum_required(VERSION 3.1...3.16.8 FATAL_ERROR)

find_package(spdlog REQUIRED)
find_package(cppzmq REQUIRED)

add_executable( zmq_client "./src/zmq_client.cc" )

set_target_properties( zmq_client
                       PROPERTIES
                       CXX_STANDARD 20
                       CXX_EXTENSIONS OFF
                       CXX_STANDARD_REQUIRED ON
                       CXX_POSITION_INDEPENDENT_CODE ON )

target_include_directories( zmq_client
                            PUBLIC
                            "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries( zmq_client
                       PUBLIC
                       spdlog::spdlog
                       cppzmq
                       tcmalloc )


add_executable( zmq_server "./src/zmq_server.cc" )

set_target_properties( zmq_server
                       PROPERTIES
                       CXX_STANDARD 20
                       CXX_EXTENSIONS OFF
                       CXX_STANDARD_REQUIRED ON
                       CXX_POSITION_INDEPENDENT_CODE ON )

target_include_directories( zmq_server
                            PUBLIC
                            "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries( zmq_server
                       PUBLIC
                       spdlog::spdlog
                       cppzmq
                       tcmalloc )

add_executable( zmq_radio "./src/zmq_radio.cc" )

set_target_properties( zmq_radio
                       PROPERTIES
                       CXX_STANDARD 20
                       CXX_EXTENSIONS OFF
                       CXX_STANDARD_REQUIRED ON
                       CXX_POSITION_INDEPENDENT_CODE ON )

target_include_directories( zmq_radio
                            PUBLIC
                            "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries( zmq_radio
                       PUBLIC
                       spdlog::spdlog
                       cppzmq
                       tcmalloc )

add_executable( zmq_dish "./src/zmq_dish.cc" )

set_target_properties( zmq_dish
                       PROPERTIES
                       CXX_STANDARD 20
                       CXX_EXTENSIONS OFF
                       CXX_STANDARD_REQUIRED ON
                       CXX_POSITION_INDEPENDENT_CODE ON )

target_include_directories( zmq_dish
                            PUBLIC
                            "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries( zmq_dish
                       PUBLIC
                       spdlog::spdlog
                       cppzmq
                       tcmalloc )

add_executable( zmq_pub "./src/zmq_pub.cc" )

set_target_properties( zmq_pub
                       PROPERTIES
                       CXX_STANDARD 20
                       CXX_EXTENSIONS OFF
                       CXX_STANDARD_REQUIRED ON
                       CXX_POSITION_INDEPENDENT_CODE ON )

target_include_directories( zmq_pub
                            PUBLIC
                            "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries( zmq_pub
                       PUBLIC
                       spdlog::spdlog
                       cppzmq
                       tcmalloc )

add_executable( zmq_sub "./src/zmq_sub.cc" )

set_target_properties( zmq_sub
                       PROPERTIES
                       CXX_STANDARD 20
                       CXX_EXTENSIONS OFF
                       CXX_STANDARD_REQUIRED ON
                       CXX_POSITION_INDEPENDENT_CODE ON )

target_include_directories( zmq_sub
                            PUBLIC
                            "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries( zmq_sub
                       PUBLIC
                       spdlog::spdlog
                       cppzmq
                       tcmalloc )
