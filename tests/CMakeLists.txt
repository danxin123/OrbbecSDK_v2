# Copyright (c) Orbbec Inc. All Rights Reserved.
# Licensed under the MIT License.

cmake_minimum_required(VERSION 3.14)

# ---------------------------------------------------------------------------
# GoogleTest via FetchContent (no system install required)
# ---------------------------------------------------------------------------
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.15.2
)
# Prevent GoogleTest from overriding compiler/linker settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# ---------------------------------------------------------------------------
# Auto-discover test sub-directories
# ---------------------------------------------------------------------------
file(GLOB subdirectories RELATIVE ${CMAKE_CURRENT_LIST_DIR} "*")

foreach(subdir ${subdirectories})
    if(IS_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/${subdir} AND EXISTS ${CMAKE_CURRENT_LIST_DIR}/${subdir}/CMakeLists.txt)
        add_subdirectory(${subdir})
    endif()
endforeach()

