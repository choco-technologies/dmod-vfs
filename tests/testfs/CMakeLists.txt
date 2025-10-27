cmake_minimum_required(VERSION 3.18)

# DMOD_DIR must be set (either via command line or environment variable)
if(NOT DEFINED DMOD_DIR)
    if(DEFINED ENV{DMOD_DIR})
        set(DMOD_DIR $ENV{DMOD_DIR})
    else()
        message(FATAL_ERROR "DMOD_DIR is not set. Please set it to the path of the DMOD repository")
    endif()
endif()
set(DMOD_MODE "DMOD_MODULE" CACHE STRING "Mode" FORCE)
set(DMOD_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}")

include(${DMOD_DIR}/paths.cmake)

project(testfs_module)

dmod_setup_external_module()

# Name of the module 
set(DMOD_MODULE_NAME        testfs)

# Version (should be string in format "Major.Minor") 
set(DMOD_MODULE_VERSION     "0.1")

# Author (should be string)
set(DMOD_AUTHOR_NAME        Patryk Kubiak)

# Stack size for the module (should be integer)
set(DMOD_STACK_SIZE         1024)

# ======================================================================
#               Fetch DMOD File System Interface
# ======================================================================
include(FetchContent)
FetchContent_Declare(
    dmfsi
    GIT_REPOSITORY
    https://github.com/choco-technologies/dmfsi.git
    GIT_TAG        master
)
FetchContent_MakeAvailable(dmfsi)
set(DMOD_DIF_IMPLS dmfsi)

#
#   dmod_add_library - create a library module
#   it has the same signature as add_library
#   and can be used in the same way after the creation
#   (for example, to link libraries)
#
dmod_add_library(${DMOD_MODULE_NAME} ${DMOD_MODULE_VERSION}
    # List of source files - can include C and C++ files
    testfs.c
)

# Link to DMFSI interface
target_link_libraries(${DMOD_MODULE_NAME} dmfsi_if)
