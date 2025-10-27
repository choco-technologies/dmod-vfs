cmake_minimum_required(VERSION 3.10)

# Set the project name
project(dmvfs_example VERSION 1.0 DESCRIPTION "DMOD VFS Example" LANGUAGES C)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the executable with all test files
add_executable(${PROJECT_NAME} 
    main.c
    test_framework.c
    test_file_operations.c
    test_directory_operations.c
    test_path_operations.c
)

# Include directories
include_directories(${PROJECT_SOURCE_DIR})

# Link libraries (including testfs for testing)
# Use whole-archive to ensure testfs registration symbols are included
# Link testfs with whole-archive, and ensure it comes before other libraries
if(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} 
        -Wl,--whole-archive testfs -Wl,--no-whole-archive
        dmod dmvfs
        -Wl,--allow-multiple-definition
    )
else()
    target_link_libraries(${PROJECT_NAME} testfs dmod dmvfs)
endif()


target_link_options(${PROJECT_NAME} PRIVATE -L ${DMOD_DIR}/scripts)
target_link_options(${PROJECT_NAME} PRIVATE -T ${CMAKE_CURRENT_SOURCE_DIR}/main.ld)
