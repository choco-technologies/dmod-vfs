cmake_minimum_required(VERSION 3.14)

project(dmod_vfs
    VERSION 0.1.0
    DESCRIPTION "DMOD VFS module"
    LANGUAGES C
)

include(FetchContent)

option(DMOD_STANDALONE "Fetch dmod dependency automatically" OFF)

if(DMOD_STANDALONE)
    set(DMOD_BUILD_EXAMPLES OFF CACHE BOOL "Disable dmod examples")
    set(DMOD_BUILD_TESTS OFF CACHE BOOL "Disable dmod tests")
    set(DMOD_BUILD_TOOLS OFF CACHE BOOL "Disable dmod tools")
    
    FetchContent_Declare(
        dmod
        GIT_REPOSITORY git@github.com:choco-technologies/dmod.git
        GIT_TAG develop
    )
    
    FetchContent_MakeAvailable(dmod)
    
    set(DMOD_DIR ${dmod_SOURCE_DIR})
    set(DMOD_BUILD_DIR ${dmod_BINARY_DIR})
endif()

set(MODULE_NAME dmod_vfs)
set(DMOD_MODE "DMOD_SYSTEM")
add_library(${MODULE_NAME} STATIC
    src/dmod_vfs.c
)

target_include_directories(${MODULE_NAME} 
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/inc
)   

target_link_libraries(${MODULE_NAME} 
    PUBLIC    
        dmod
    )

create_library_makefile(${MODULE_NAME})