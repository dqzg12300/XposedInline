cmake_minimum_required(VERSION 3.4.1)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")
aux_source_directory(src SRC_LIST)

add_library(substrate STATIC ${SRC_LIST})

target_include_directories(substrate INTERFACE src)