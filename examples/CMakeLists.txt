# Copyright (c) Orbbec Inc. All Rights Reserved.
# Licensed under the MIT License.

cmake_minimum_required(VERSION 3.10)

file(GLOB subdirectories RELATIVE ${CMAKE_CURRENT_LIST_DIR} "*")

# dependecies
if(NOT DEFINED OpenCV_DIR OR OpenCV_DIR STREQUAL "")
    set(_opencv_candidate_dirs
        "D:/tools/opencv/build/x64/vc16/lib"
        "D:/tools/opencv/build"
    )

    foreach(_opencv_dir IN LISTS _opencv_candidate_dirs)
        if(EXISTS "${_opencv_dir}/OpenCVConfig.cmake")
            set(OpenCV_DIR "${_opencv_dir}" CACHE PATH "Path to OpenCVConfig.cmake" FORCE)
            message(STATUS "OpenCV_DIR auto-set to: ${OpenCV_DIR}")
            break()
        endif()
    endforeach()
endif()

find_package(OpenCV QUIET)
if(NOT OpenCV_FOUND)
    message(WARNING "OpenCV not found, some examples may not be built. Please install OpenCV or set OpenCV_DIR to the directory of your OpenCV installation.")
endif()

# utils
add_subdirectory(utils)

# c examples
add_subdirectory(c_examples)

# ============================================================================
# Beginner examples (no OpenCV required)
# ============================================================================
add_subdirectory(beginner/quick_start)
add_subdirectory(beginner/enumerate_control)
add_subdirectory(beginner/imu)

# ============================================================================
# Advanced examples (no OpenCV required)
# ============================================================================
add_subdirectory(advanced/camera_params)
add_subdirectory(advanced/forceip)
add_subdirectory(advanced/optional_depth_presets_update)
add_subdirectory(beginner/hot_plugin)
add_subdirectory(advanced/preset)
add_subdirectory(advanced/metadata)
if(OB_BUILD_LINUX)
    add_subdirectory(advanced/multi_devices_sync_gmsltrigger)
endif()

if (OB_BUILD_PCL_EXAMPLES)
    add_subdirectory(advanced/wrapper_pcl)
endif()

if (OB_BUILD_OPEN3D_EXAMPLES)
    add_subdirectory(advanced/wrapper_open3d)
endif()

# ============================================================================
# Beginner examples (OpenCV required)
# ============================================================================
if(${OpenCV_FOUND})
    add_subdirectory(beginner/depth_viewer)
    add_subdirectory(beginner/record_playback)
    add_subdirectory(beginner/rgbd_viewer)
endif()

# ============================================================================
# Advanced examples (OpenCV required)
# ============================================================================
if(${OpenCV_FOUND})
    add_subdirectory(advanced/align_modes)
    add_subdirectory(advanced/infrared_streams)
    add_subdirectory(advanced/callback)
    add_subdirectory(advanced/confidence)
    add_subdirectory(advanced/post_processing)
    add_subdirectory(advanced/hdr)
    add_subdirectory(advanced/laser_interleave)
    add_subdirectory(advanced/multi_devices_sync)
    add_subdirectory(advanced/save_to_disk)
endif()

# ============================================================================
# Application examples (OpenCV required)
# ============================================================================
if(${OpenCV_FOUND})
    add_subdirectory(application/3d_measurement)
endif()

# lidar examples
add_subdirectory(lidar_examples)

# InstallRequiredSystemLibraries
include(InstallRequiredSystemLibraries)
