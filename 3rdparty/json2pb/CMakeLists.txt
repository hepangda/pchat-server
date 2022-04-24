cmake_minimum_required(VERSION 3.0)
project(json2pb)

set(CMAKE_CXX_STANDARD 20)
add_definitions("-O3")

include_directories(.)

add_library(json2pb
        json2pb.cc
        json2pb.h
        bin2ascii.h)

find_package(jansson CONFIG REQUIRED)
target_link_libraries(json2pb PRIVATE jansson::jansson)