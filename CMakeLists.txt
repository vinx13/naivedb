cmake_minimum_required(VERSION 3.1)
project(naivedb)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")


set(SOURCE_FILES
        src/mmap.cpp
        src/mmap_posix.cpp
        src/mmap_windows.cpp
        src/datafile.cpp
        src/dbstore.cpp
        src/file.cpp
        src/indexfile.cpp
        src/bplusnode.cpp
        src/bplus.cpp
        src/database.cpp
        src/platform.h
        )

set(TEST_FILES
        test/test_mmap.cpp
        test/test_storage.cpp
        test/test_lrumap.cpp
        test/test_bplus.cpp
        )

add_library(naivedb ${SOURCE_FILES})

add_subdirectory("extern/googletest" "${CMAKE_CURRENT_BINARY_DIR}/googletest")
include_directories("extern/googletest/include" src)

add_executable(unittest ${TEST_FILES})
target_link_libraries(unittest gtest gtest_main naivedb)

if (WIN32)
    target_link_libraries(naivedb Shlwapi)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
endif ()

cmake_policy(PUSH)
if (POLICY CMP0037)
    cmake_policy(SET CMP0037 OLD)
endif ()
add_custom_target(test make && ./unittest)
cmake_policy(POP)
