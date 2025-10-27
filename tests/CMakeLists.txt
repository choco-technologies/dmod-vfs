# =====================================================================
#               DMVFS Tests
# =====================================================================
cmake_minimum_required(VERSION 3.10)

# Enable testing
enable_testing()

# Include directories for tests
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/inc
)

# Get DMOD directory from parent scope
set(DMOD_SCRIPTS_DIR ${dmod_SOURCE_DIR}/scripts)

# Configure the ramfs wrapper CMakeLists
set(DMFSI_SOURCE_DIR "${dmfsi_SOURCE_DIR}")
set(DMFSI_BUILD_DIR "${dmfsi_BINARY_DIR}")
set(DMFSI_RAMFS_DIR "${dmfsi_SOURCE_DIR}/examples/ramfs")
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/ramfs_wrapper/CMakeLists.txt.in
    ${CMAKE_CURRENT_BINARY_DIR}/ramfs_wrapper/CMakeLists.txt
    @ONLY
)

# Build ramfs as a DMF module using ExternalProject
include(ExternalProject)
ExternalProject_Add(ramfs_module
    SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/ramfs_wrapper
    BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/ramfs_build
    CMAKE_ARGS
        -DDMOD_DIR=${dmod_SOURCE_DIR}
        -DDMOD_MODE=DMOD_MODULE
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    BUILD_ALWAYS 1
    INSTALL_COMMAND ""
)

# Create the integration test executable
add_executable(test_dmvfs_integration test_dmvfs_integration.c dmfsi_sigs_helper.c)

# Link libraries in proper order (dmvfs and dependencies)
target_link_libraries(test_dmvfs_integration dmvfs dmfsi dmod)

# Use the main linker script from DMOD
target_link_options(test_dmvfs_integration PRIVATE 
    -T${DMOD_SCRIPTS_DIR}/main.ld
    -L${DMOD_SCRIPTS_DIR}
    -no-pie
)

# Make sure the test depends on ramfs being built
add_dependencies(test_dmvfs_integration ramfs_module)

# The ramfs.dmf will be built in the ramfs_build/dmf directory
set(RAMFS_DMF_PATH ${CMAKE_CURRENT_BINARY_DIR}/ramfs_build/dmf/ramfs.dmf)

# Add test that loads ramfs.dmf dynamically
add_test(NAME test_dmvfs_integration 
    COMMAND test_dmvfs_integration ${RAMFS_DMF_PATH}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

# Custom target to run all tests
add_custom_target(run_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS test_dmvfs_integration
    COMMENT "Running DMVFS integration tests"
)
