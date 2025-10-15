cmake_minimum_required(VERSION 3.18)
project(dmod-vfs VERSION 1.0.0 LANGUAGES C)

# Project configuration
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Options
option(DMOD_VFS_BUILD_EXAMPLES "Build examples" ON)

# DMOD configuration
if(NOT DEFINED DMOD_DIR)
    set(DMOD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../dmod" CACHE PATH "Path to DMOD installation")
endif()

# Find DMOD include directory
if(NOT EXISTS "${DMOD_DIR}/inc")
    message(FATAL_ERROR "DMOD directory not found at: ${DMOD_DIR}")
endif()

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${DMOD_DIR}/inc
)

# Add compile definitions
# Note: We don't set DMOD_SYSTEM_EN as we provide our own implementations

# Source files
file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")

# Create static library
add_library(${PROJECT_NAME} STATIC ${SOURCES})

# Set library properties
set_target_properties(${PROJECT_NAME} PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/inc/dmod_vfs.h"
)

# Installation rules
install(TARGETS ${PROJECT_NAME}
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    PUBLIC_HEADER DESTINATION include/${PROJECT_NAME}
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/inc/
    DESTINATION include/${PROJECT_NAME}
    FILES_MATCHING PATTERN "*.h"
)

# Examples (optional)
if(DMOD_VFS_BUILD_EXAMPLES)
    # Add examples subdirectory if it exists and has CMakeLists.txt
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/examples/CMakeLists.txt")
        add_subdirectory(examples)
    endif()
endif()

# Package configuration
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "DMOD VFS - Virtual File System for DMOD")
set(CPACK_PACKAGE_VENDOR "Choco-Technologies")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

include(CPack)
