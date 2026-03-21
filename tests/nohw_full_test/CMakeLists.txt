# Copyright (c) Orbbec Inc. All Rights Reserved.
# Licensed under the MIT License.

cmake_minimum_required(VERSION 3.14)

add_executable(ob_nohw_full_test nohw_full_test.cpp)

target_link_libraries(ob_nohw_full_test
    PRIVATE
        ob::OrbbecSDK
        GTest::gtest
)

target_include_directories(ob_nohw_full_test
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# CTest integration
include(GoogleTest)
gtest_discover_tests(ob_nohw_full_test
    PROPERTIES LABELS "nohw"
    DISCOVERY_TIMEOUT 30
)

set_target_properties(ob_nohw_full_test PROPERTIES FOLDER "tests")
if(MSVC)
    set_target_properties(ob_nohw_full_test PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
endif()

install(TARGETS ob_nohw_full_test RUNTIME DESTINATION bin)
