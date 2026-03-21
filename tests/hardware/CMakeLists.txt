# Copyright (c) Orbbec Inc. All Rights Reserved.
# Licensed under the MIT License.

cmake_minimum_required(VERSION 3.10)

add_executable(ob_hardware_p0_test hardware_p0_test.cpp)
target_link_libraries(ob_hardware_p0_test PRIVATE ob::OrbbecSDK)
set_target_properties(ob_hardware_p0_test PROPERTIES FOLDER "tests")
