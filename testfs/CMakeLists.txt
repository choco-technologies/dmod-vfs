cmake_minimum_required(VERSION 3.10)

# TestFS is a simple static library for testing
# It implements the DMFSI interface

set(DMOD_MODULE_NAME testfs)
set(DMOD_MODULE_VERSION "1.0")

# Build as a regular static library
add_library(${DMOD_MODULE_NAME} STATIC
    testfs.c
)

# Create interface library for consistency
add_library(${DMOD_MODULE_NAME}_if INTERFACE)

target_include_directories(${DMOD_MODULE_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
)

target_include_directories(${DMOD_MODULE_NAME}_if
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(${DMOD_MODULE_NAME} PUBLIC dmod dmfsi_if)
target_link_libraries(${DMOD_MODULE_NAME}_if INTERFACE ${DMOD_MODULE_NAME})

# Generate the _defs.h file for interface definitions
# Use DMOD_DIR which should be set by parent CMakeLists
if(DEFINED DMOD_DIR)
    set(DMOD_SCRIPTS_DIR ${DMOD_DIR}/scripts)
    set(DMOD_MODULE_NAME_SNAKE_CASE "testfs")
    set(DMOD_MODULE_TYPE "Library")
    configure_file(${DMOD_SCRIPTS_DIR}/api.h.in ${CMAKE_CURRENT_BINARY_DIR}/${DMOD_MODULE_NAME_SNAKE_CASE}_defs.h)
endif()

# Add DIF implementation definition
target_compile_definitions(${DMOD_MODULE_NAME}
    PRIVATE
        DMOD_DIF_dmfsi
)

