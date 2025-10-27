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

# Create mock filesystem signatures library
add_library(mock_fs_sigs STATIC mock_fs_sigs.c)
target_link_libraries(mock_fs_sigs dmfsi)

# Test executable macro with proper linking order and linker script
macro(add_dmvfs_test test_name test_source)
    add_executable(${test_name} ${test_source})
    # Link libraries in proper order, including mock FS signatures
    target_link_libraries(${test_name} dmvfs mock_fs_sigs dmfsi dmod)
    # Use the main linker script from DMOD
    target_link_options(${test_name} PRIVATE 
        -T${DMOD_SCRIPTS_DIR}/main.ld
        -L${DMOD_SCRIPTS_DIR}
        -no-pie
    )
    add_test(NAME ${test_name} COMMAND ${test_name})
endmacro()

# Add individual test executables
add_dmvfs_test(test_init test_init.c)
add_dmvfs_test(test_path test_path.c)
add_dmvfs_test(test_file_ops test_file_ops.c)
add_dmvfs_test(test_mount test_mount.c)
add_dmvfs_test(test_dir_ops test_dir_ops.c)

# Custom target to run all tests
add_custom_target(run_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS test_init test_path test_file_ops test_mount test_dir_ops
    COMMENT "Running all DMVFS tests"
)
