cmake_minimum_required(VERSION 3.14)

# ============================================================
# ob_scenario_test — Application scenario tests (require device)
# ============================================================

add_executable(ob_scenario_test
    scenario_test_main.cpp
    log_completeness_test.cpp
)

target_include_directories(ob_scenario_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..   # test_common.hpp
)

target_link_libraries(ob_scenario_test PRIVATE
    ob::OrbbecSDK
    GTest::gtest
)

target_compile_features(ob_scenario_test PRIVATE cxx_std_14)

include(GoogleTest)
gtest_discover_tests(ob_scenario_test
    PROPERTIES LABELS "scenario"
)
