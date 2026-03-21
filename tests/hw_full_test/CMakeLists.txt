cmake_minimum_required(VERSION 3.14)

# ============================================================
# ob_hw_full_test — 硬件全量测试 (需要物理设备)
# ============================================================

add_executable(ob_hw_full_test
    hw_full_test.cpp
)

target_include_directories(ob_hw_full_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..   # test_common.hpp
)

target_link_libraries(ob_hw_full_test PRIVATE
    ob::OrbbecSDK
    GTest::gtest
)

target_compile_features(ob_hw_full_test PRIVATE cxx_std_14)

# CTest 自动发现
include(GoogleTest)
gtest_discover_tests(ob_hw_full_test
    PROPERTIES LABELS "hw"
)

set_target_properties(ob_hw_full_test PROPERTIES FOLDER "tests")
if(MSVC)
    set_target_properties(ob_hw_full_test PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
endif()

install(TARGETS ob_hw_full_test RUNTIME DESTINATION bin)
