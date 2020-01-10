project(ledger-core-cosmos-tests)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(gtest_force_shared_crt ON CACHE BOOL "Build gtest with shared runtime" FORCE)

add_subdirectory(lib/googletest)
add_subdirectory(lib/libledger-test)

if (APPLE)
    add_definitions(-DGTEST_USE_OWN_TR1_TUPLE)
    add_definitions(-D__GLIBCXX__)
endif (APPLE)

add_executable(${PROJECT_NAME} main.cpp address_test.cpp transactions_test.cpp)

target_link_libraries(${PROJECT_NAME} gtest gtest_main)
target_link_libraries(${PROJECT_NAME} ledger-core)
target_link_libraries(${PROJECT_NAME} ledger-test)
target_link_libraries(${PROJECT_NAME} leveldb)
# target_include_directories(${PROJECT_NAME} PUBLIC ../../../core/lib/leveldb/include)
include_directories(../lib/libledger-test/)

include(CopyAndInstallImportedTargets)
copy_install_imported_targets(${PROJECT_NAME} crypto ledger-core)

add_test (NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
