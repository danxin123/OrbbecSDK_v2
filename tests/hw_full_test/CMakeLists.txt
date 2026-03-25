cmake_minimum_required(VERSION 3.14)

# ============================================================
# ob_hw_full_test — 硬件全量测试 (需要物理设备)
# ============================================================

set(OB_HW_DESTRUCTIVE_TEST_FILTER
    "TC_CPP_02_Discovery_HW.TC_CPP_02_04_force_ip:TC_CPP_14_Property_HW.TC_CPP_14_07_customer_data:TC_CPP_21_Firmware.TC_CPP_21_08_firmware_update:TC_CPP_21_Firmware.TC_CPP_21_09_update_depth_presets"
)
set(OB_HW_NON_DESTRUCTIVE_TEST_FILTER "*-${OB_HW_DESTRUCTIVE_TEST_FILTER}")

add_executable(ob_hw_full_test
    hw_full_test.cpp
    hw_full_test_main.cpp
)

add_executable(ob_hw_destructive_test
    hw_full_test.cpp
    hw_destructive_test_main.cpp
)

target_include_directories(ob_hw_full_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..   # test_common.hpp
)

target_include_directories(ob_hw_destructive_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..   # test_common.hpp
)

target_link_libraries(ob_hw_full_test PRIVATE
    ob::OrbbecSDK
    GTest::gtest
)

target_link_libraries(ob_hw_destructive_test PRIVATE
    ob::OrbbecSDK
    GTest::gtest
)

target_compile_features(ob_hw_full_test PRIVATE cxx_std_14)
target_compile_features(ob_hw_destructive_test PRIVATE cxx_std_14)

# CTest 自动发现
include(GoogleTest)
gtest_discover_tests(ob_hw_full_test
    TEST_FILTER ${OB_HW_NON_DESTRUCTIVE_TEST_FILTER}
    PROPERTIES LABELS "hw"
)

gtest_discover_tests(ob_hw_destructive_test
    TEST_FILTER ${OB_HW_DESTRUCTIVE_TEST_FILTER}
    PROPERTIES LABELS "hw-destructive"
)

set_target_properties(ob_hw_full_test PROPERTIES FOLDER "tests")
set_target_properties(ob_hw_destructive_test PROPERTIES FOLDER "tests")
if(MSVC)
    set_target_properties(ob_hw_full_test PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
    set_target_properties(ob_hw_destructive_test PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
endif()

install(TARGETS ob_hw_full_test RUNTIME DESTINATION bin)
install(TARGETS ob_hw_destructive_test RUNTIME DESTINATION bin)
