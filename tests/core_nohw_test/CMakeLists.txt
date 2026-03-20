# Copyright (c) Orbbec Inc. All Rights Reserved.
# Licensed under the MIT License.

cmake_minimum_required(VERSION 3.10)

add_executable(ob_core_nohw_test core_nohw_test.cpp)
target_link_libraries(ob_core_nohw_test PRIVATE ob::OrbbecSDK)
set_target_properties(ob_core_nohw_test PROPERTIES FOLDER "tests")
