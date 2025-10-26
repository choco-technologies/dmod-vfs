# =====================================================================
#               DMOD Virtual File System
# =====================================================================
cmake_minimum_required(VERSION 3.10)

# ======================================================================
#               DMOD VFS
# ======================================================================
project(dmod-vfs 
    VERSION 1.0
    DESCRIPTION "DMOD Virtual File System"
    LANGUAGES C CXX)

# ======================================================================
#               Fetch DMOD repository
# ======================================================================
include(FetchContent)
FetchContent_Declare(
    dmod
    GIT_REPOSITORY https://github.com/choco-technologies/dmod.git
    GIT_TAG        develop
)

# ======================================================================
#               DMOD Configuration
# ======================================================================
set(DMOD_MODE "DMOD_SYSTEM" CACHE STRING "DMOD build mode")
set(DMOD_BUILD_TESTS OFF CACHE BOOL "Build tests")
set(DMOD_BUILD_EXAMPLES OFF CACHE BOOL "Build examples")
set(DMOD_BUILD_TOOLS OFF CACHE BOOL "Build tools")
set(DMOD_BUILD_TEMPLATES OFF CACHE BOOL "Build templates")

FetchContent_MakeAvailable(dmod)

# ======================================================================
#               Fetch DMOD File System Interface
# ======================================================================
FetchContent_Declare(
    dmod-fsi
    GIT_REPOSITORY
    https://github.com/choco-technologies/dmod-fsi.git
    GIT_TAG        master
)

# ======================================================================
#               DMOD File System Interface Configuration
# ======================================================================
set(DMOD_DIR ${dmod_SOURCE_DIR})
FetchContent_MakeAvailable(dmod-fsi)

# ======================================================================
#               DMOD VFS Library definition
# ======================================================================
add_library(dmod-vfs STATIC
    src/dmod_vfs.c
)

target_include_directories(dmod-vfs PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
)

target_link_libraries(dmod-vfs PUBLIC
    dmod
    dmod-fsi
)