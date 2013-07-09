enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(alloctest src/alloctest.cc tests/test_bf.cc tests/test_cff.cc tests/test_run_cpp.cc tests/test_buddy.cc tests/test_generic.cc)
target_link_libraries( ${GTEST_BOTH_LIBRARIES})

add_test(AllTestsAlocator alloctest)
