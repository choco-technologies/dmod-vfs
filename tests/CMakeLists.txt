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
set(DMOD_DIR ${dmod_SOURCE_DIR})

# Build the RamFS module
add_subdirectory(ramfs)

# Create the integration test executable
add_executable(test_dmvfs_integration test_dmvfs_integration.c)

# Link libraries in proper order
target_link_libraries(test_dmvfs_integration dmvfs ramfs dmfsi dmod)

# Use the main linker script from DMOD and allow multiple definitions
target_link_options(test_dmvfs_integration PRIVATE 
    -T${DMOD_SCRIPTS_DIR}/main.ld
    -L${DMOD_SCRIPTS_DIR}
    -no-pie
    -Wl,--allow-multiple-definition
)

# Add test
add_test(NAME test_dmvfs_integration COMMAND test_dmvfs_integration)

# Custom target to run all tests
add_custom_target(run_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS test_dmvfs_integration
    COMMENT "Running DMVFS integration tests"
)
