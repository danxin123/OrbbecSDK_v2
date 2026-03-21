# Copyright (c) Orbbec Inc. All Rights Reserved.
# Licensed under the MIT License.

cmake_minimum_required(VERSION 3.10)

file(GLOB subdirectories RELATIVE ${CMAKE_CURRENT_LIST_DIR} "*")

# dependecies
find_package(OpenCV QUIET)
if(NOT ${OpenCV_FOUND})
    message(WARNING "OpenCV not found, some examples may not be built. Please install OpenCV or set OpenCV_DIR to the directory of your OpenCV installation.")
endif()

# utils
add_subdirectory(utils)

# c examples
add_subdirectory(c_examples)

# ============================================================================
# Beginner examples (no OpenCV required)
# ============================================================================
add_subdirectory(beginner/04_enumerate)
add_subdirectory(beginner/05_point_cloud)
add_subdirectory(beginner/07_imu)
add_subdirectory(beginner/09_firmware_update)

# ============================================================================
# Advanced examples (no OpenCV required)
# ============================================================================
add_subdirectory(advanced/control)
add_subdirectory(advanced/forceip)
add_subdirectory(advanced/multi_devices_firmware_update)
add_subdirectory(advanced/optional_depth_presets_update)
add_subdirectory(advanced/record_nogui)
add_subdirectory(advanced/hot_plugin)
add_subdirectory(advanced/preset)
add_subdirectory(advanced/logger)
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
    add_subdirectory(beginner/01_quick_start)
    add_subdirectory(beginner/02_depth_viewer)
    add_subdirectory(beginner/03_color_viewer)
    add_subdirectory(beginner/06_multi_streams)
    add_subdirectory(beginner/08_infrared)
endif()

# ============================================================================
# Advanced examples (OpenCV required)
# ============================================================================
if(${OpenCV_FOUND})
    add_subdirectory(advanced/callback)
    add_subdirectory(advanced/confidence)
    add_subdirectory(advanced/decimation)
    add_subdirectory(advanced/record)
    add_subdirectory(advanced/playback)
    add_subdirectory(advanced/common_usages)
    add_subdirectory(advanced/sync_align)
    add_subdirectory(advanced/hw_d2c_align)
    add_subdirectory(advanced/post_processing)
    add_subdirectory(advanced/coordinate_transform)
    add_subdirectory(advanced/hdr)
    add_subdirectory(advanced/laser_interleave)
    add_subdirectory(advanced/multi_devices)
    add_subdirectory(advanced/multi_devices_sync)
    add_subdirectory(advanced/save_to_disk)
    add_subdirectory(advanced/wrapper_opencv)
endif()

# lidar examples
add_subdirectory(lidar_examples)

# InstallRequiredSystemLibraries
include(InstallRequiredSystemLibraries)
