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

if (WIN32)
    target_link_libraries(naivedb Shlwapi)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
endif ()

add_subdirectory("extern/googletest" "${CMAKE_CURRENT_BINARY_DIR}/googletest")
add_subdirectory("extern/benchmark" "${CMAKE_CURRENT_BINARY_DIR}/benchmark")
include_directories("extern/googletest/include" "extern/benchmark/include" src)

add_executable(example example/main.cpp)
target_link_libraries(example naivedb)

add_executable(unittest ${TEST_FILES})
target_link_libraries(unittest gtest gtest_main naivedb)

add_executable(db_benchmark benchmark/benchmark.cpp benchmark/util.cpp)
add_executable(db_benchmark2 benchmark/benchmark2.cpp benchmark/util.cpp)
target_link_libraries(db_benchmark naivedb benchmark)
target_link_libraries(db_benchmark2 naivedb benchmark)

cmake_policy(PUSH)
if (POLICY CMP0037)
    cmake_policy(SET CMP0037 OLD)
endif ()
add_custom_target(test make && ./unittest)
cmake_policy(POP)
