cmake_minimum_required(VERSION 3.14)

# ============================================================
# ob_perf_test — 性能测试 (需要物理设备)
# ============================================================

add_executable(ob_perf_test
    perf_test.cpp
    thread_safety_test.cpp
    perf_test_main.cpp
)

target_include_directories(ob_perf_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..   # test_common.hpp
)

target_link_libraries(ob_perf_test PRIVATE
    ob::OrbbecSDK
    GTest::gtest
)

if(WIN32)
    target_link_libraries(ob_perf_test PRIVATE psapi)
endif()

target_compile_features(ob_perf_test PRIVATE cxx_std_14)

include(GoogleTest)
gtest_discover_tests(ob_perf_test
    PROPERTIES LABELS "perf"
)
