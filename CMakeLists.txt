cmake_minimum_required(VERSION 2.4)
project(Allocphy)
enable_testing()
add_definitions(-Wall -Werror)

if(EXISTS /usr/src/gtest)
    add_subdirectory(/usr/src/gtest ./gtest)
else(EXISTS /usr/src/gtest)
    find_package(GTest REQUIRED)
    include_directories(${GTEST_INCLUDE_DIRS})
endif(EXISTS /usr/src/gtest)

add_library(liballocphy STATIC src/mem.c)
add_executable(alloctest src/alloctest.cc tests/test_bf.cc tests/test_cff.cc  tests/test_buddy.cc tests/test_generic.cc tests/test_run_cpp.cc)
target_link_libraries(alloctest gtest gtest_main liballocphy)
add_test(AllTestsAlocator alloctest)

