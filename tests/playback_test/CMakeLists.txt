# Copyright (c) Orbbec Inc. All Rights Reserved.
# Licensed under the MIT License.

cmake_minimum_required(VERSION 3.10)

add_executable(ob_playback_smoke_test playback_smoke_test.cpp)
target_link_libraries(ob_playback_smoke_test PRIVATE ob::OrbbecSDK)
set_target_properties(ob_playback_smoke_test PROPERTIES FOLDER "tests")
if(MSVC)
	set_target_properties(ob_playback_smoke_test PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
endif()

install(TARGETS ob_playback_smoke_test RUNTIME DESTINATION bin)
