cmake_minimum_required(VERSION 2.8)
project(medb)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall-std=c++11")

set(SOURCE_FILES
        src/database.cpp
        src/storage.cpp
        )
add_library(medb ${SOURCE_FILES})