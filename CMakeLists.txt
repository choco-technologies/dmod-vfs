# =====================================================================
#               DMOD Virtual File System
# =====================================================================
cmake_minimum_required(VERSION 3.10)

# ======================================================================
#               DMOD VFS
# ======================================================================
project(dmvfs 
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
#               Apply DMOD patches
# ======================================================================
# Apply fix for mprotect bug in dmod (sizes larger than one page)
# This patch ensures that memory protection is applied to all allocated
# pages, not just the first one. Required for modules > 4KB.
# See docs/dmod-mprotect-fix.md for details.
set(DMOD_PATCH_FILE "${CMAKE_CURRENT_SOURCE_DIR}/patches/dmod-mprotect-fix.patch")
set(DMOD_PATCH_TARGET "${dmod_SOURCE_DIR}/src/system/if/dmod_if_mem.c")
set(DMOD_PATCH_MARKER "${dmod_SOURCE_DIR}/.patch_applied")

if(EXISTS "${DMOD_PATCH_FILE}" AND NOT EXISTS "${DMOD_PATCH_MARKER}")
    message(STATUS "Applying DMOD mprotect fix patch...")
    execute_process(
        COMMAND git apply "${DMOD_PATCH_FILE}"
        WORKING_DIRECTORY "${dmod_SOURCE_DIR}"
        RESULT_VARIABLE PATCH_RESULT
        OUTPUT_VARIABLE PATCH_OUTPUT
        ERROR_VARIABLE PATCH_ERROR
    )
    if(PATCH_RESULT EQUAL 0)
        message(STATUS "DMOD patch applied successfully")
        file(WRITE "${DMOD_PATCH_MARKER}" "Patch applied at ${CMAKE_CURRENT_LIST_FILE}")
    else()
        message(WARNING "Failed to apply DMOD patch: ${PATCH_ERROR}")
    endif()
endif()

# ======================================================================
#               Fetch DMOD File System Interface
# ======================================================================
FetchContent_Declare(
    dmfsi
    GIT_REPOSITORY
    https://github.com/choco-technologies/dmfsi.git
    GIT_TAG        master
)

# ======================================================================
#               DMOD File System Interface Configuration
# ======================================================================
set(DMOD_DIR ${dmod_SOURCE_DIR})
FetchContent_MakeAvailable(dmfsi)

# ======================================================================
#               DMOD VFS Library definition
# ======================================================================
add_library(dmvfs STATIC
    src/dmvfs.c
)

target_include_directories(dmvfs PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
)

target_link_libraries(dmvfs PUBLIC
    dmod
    dmfsi
)

add_subdirectory(tests)