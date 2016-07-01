cmake_minimum_required(VERSION 2.8)
project(medb)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall-std=c++11")

set(SOURCE_FILES
        src/database.cpp
        src/storage.cpp
        src/mmap.cpp
    )

set(TEST_FILES
    )

add_library(medb ${SOURCE_FILES})


add_executable(unittest ${TEST_FILES})

enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
target_link_libraries(unittest ${GTEST_BOTH_LIBRARIES})

add_test(alltests unittest)