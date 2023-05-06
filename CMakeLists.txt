cmake_minimum_required(VERSION 3.18)

set(ENV{VCPKG_DOWNLOADS} "${CMAKE_CURRENT_SOURCE_DIR}/assets")
set(ENV{X_VCPKG_ASSET_SOURCES} "clear;x-azurl,file://${CMAKE_CURRENT_SOURCE_DIR}/assets,,readwrite;")
set(ENV{VCPKG_OVERLAY_PORTS} "${CMAKE_CURRENT_SOURCE_DIR}/overlays")
set(ENV{VCPKG_FORCE_SYSTEM_BINARIES} 1)
set(CMAKE_TOOLCHAIN_FILE
    "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake"
    CACHE PATH "vcpkg toolchain file path"
)

project(dragon_screen)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")

if(NOT QT_DEFAULT_MAJOR_VERSION)
    set(QT_DEFAULT_MAJOR_VERSION 5 CACHE STRING "Qt version to use (5 or 6), defaults to 5")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

option(${PROJECT_NAME}_BUILD_TEST "是否启用测试" OFF)

if(${CMAKE_BUILD_TYPE}_ STREQUAL "_")
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "" FORCE)
endif()

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
  set(${PROJECT_NAME}_BUILD_TEST
      ON
      CACHE INTERNAL ""
  )
endif()

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

include(CheckCXXCompilerFlag)
check_cxx_compiler_flag(-Wl,-rpath CXX_Support_rpath)
if(${CXX_Support_rpath})
    # add_link_options(-Wl,-rpath=./:./lib)
endif()

include(FeatureSummary)

find_package(spdlog CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(Tesseract CONFIG REQUIRED)

add_subdirectory(extern)
add_subdirectory(src)

if(${${PROJECT_NAME}_BUILD_TEST})
  enable_testing()
  add_subdirectory(test)
endif()

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
