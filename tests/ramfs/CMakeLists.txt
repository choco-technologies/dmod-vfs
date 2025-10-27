cmake_minimum_required(VERSION 3.10)

set(DMOD_MODULE_NAME ramfs)

# Build as a regular static library in SYSTEM mode
add_library(${DMOD_MODULE_NAME} STATIC
    ramfs.c
)

target_include_directories(${DMOD_MODULE_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(${DMOD_MODULE_NAME} PUBLIC dmod dmfsi)

# Add DIF implementation definition
target_compile_definitions(${DMOD_MODULE_NAME}
    PRIVATE
        DMOD_DIF_dmfsi
)
