# Copyright (c) Orbbec Inc. All Rights Reserved.
# Licensed under the MIT License.

cmake_minimum_required(VERSION 3.10)
project(ob_firmware_update)

add_executable(${PROJECT_NAME} firmware_update.cpp)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)
target_link_libraries(${PROJECT_NAME} ob::${OB_SDK_LIB_NAME})

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "tools")
if(MSVC)
    set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
endif()

if(APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INSTALL_RPATH "@loader_path;@loader_path/../lib"
    )
endif()

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
