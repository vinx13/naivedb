cmake_minimum_required(VERSION 3.1)
project(naivedb)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")


set(SOURCE_FILES
        src/mmap.cpp
        src/mmap_posix.cpp
        src/datafile.cpp
        src/dbstore.cpp
        src/file.cpp
        src/indexfile.cpp
        src/bplusnode.cpp
        src/bplus.cpp
        src/lrumap.cpp
        src/database.cpp
        )

set(TEST_FILES
        test/test_mmap.cpp
        test/test_storage.cpp
        test/test_lrumap.cpp
        test/test_bplus.cpp
        )

add_library(naivedb ${SOURCE_FILES})


find_package(GTest)
if (GTEST_FOUND)
    #enable_testing()
    include_directories(${GTEST_INCLUDE_DIRS} src /usr/include)
    add_executable(unittest ${TEST_FILES})
    target_link_libraries(unittest ${GTEST_BOTH_LIBRARIES} naivedb pthread)
    #add_test(alltests unittest)

    cmake_policy(PUSH)
    if (POLICY CMP0037)
        cmake_policy(SET CMP0037 OLD)
    endif ()
    add_custom_target(test make && ./unittest)
    cmake_policy(POP)

endif ()