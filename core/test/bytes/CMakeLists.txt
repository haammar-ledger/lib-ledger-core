cmake_minimum_required(VERSION 3.0)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
include_directories(${CMAKE_BINARY_DIR}/include)
include_directories(${CMAKE_BINARY_DIR}/include/ledger/core)

if (APPLE)
    add_definitions(-DGTEST_USE_OWN_TR1_TUPLE)
    add_definitions(-D__GLIBCXX__)
endif (APPLE)

add_executable(ledger-core-bytes-tests main.cpp bytesreader_tests.cpp byteswriter_tests.cpp RLP_tests.cpp zarith.cpp)

target_link_libraries(ledger-core-bytes-tests gtest gtest_main)
target_link_libraries(ledger-core-bytes-tests ledger-core-static)

add_test (NAME ledger-core-bytes-tests COMMAND ledger-core-bytes-tests)