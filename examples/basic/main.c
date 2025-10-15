/**
 * MIT License
 * 
 * Copyright (c) 2025 Choco-Technologies
 * 
 * @brief Basic VFS Usage Example
 * 
 * This example demonstrates how to use the dmod-vfs library to:
 * - Initialize the VFS system
 * - Mount file systems
 * - Perform file operations
 * - Unmount and cleanup
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "dmod_vfs.h"

// Mock implementations of DMOD functions for standalone example
void* Dmod_Malloc(size_t size) {
    return malloc(size);
}

void Dmod_Free(void* ptr) {
    free(ptr);
}

int Dmod_Printf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    int result = vprintf(format, args);
    va_end(args);
    return result;
}

int main(void)
{
    printf("=== DMOD VFS Basic Example ===\n\n");

    // 1. Initialize VFS
    printf("Initializing VFS...\n");
    if (!DmodVfs_Init()) {
        printf("Failed to initialize VFS\n");
        return 1;
    }
    printf("VFS initialized successfully\n\n");

    // 2. Mount file systems
    // Note: This requires actual file system modules to be loaded via DMOD
    printf("Mounting file systems...\n");
    
    // Example: Mount a RAM file system at /ram
    if (DmodVfs_Mount("/ram", "ramfs")) {
        printf("Mounted ramfs at /ram\n");
    } else {
        printf("Warning: Could not mount ramfs (module may not be loaded)\n");
    }

    // Example: Mount a FAT file system at /sd
    if (DmodVfs_Mount("/sd", "fatfs")) {
        printf("Mounted fatfs at /sd\n");
    } else {
        printf("Warning: Could not mount fatfs (module may not be loaded)\n");
    }
    printf("\n");

    // 3. Check mount status
    printf("Checking mount status...\n");
    if (DmodVfs_IsMounted("/ram")) {
        printf("/ram is mounted\n");
    }
    if (DmodVfs_IsMounted("/sd")) {
        printf("/sd is mounted\n");
    }
    printf("\n");

    // 4. Use DMOD file operations (if file systems are mounted)
    printf("Testing file operations...\n");
    
    // Try to open a file (will fail if no FS is mounted)
    void* file = Dmod_FileOpen("/ram/test.txt", "w");
    if (file) {
        printf("Opened /ram/test.txt for writing\n");
        
        const char* data = "Hello, DMOD VFS!";
        size_t written = Dmod_FileWrite(data, 1, strlen(data), file);
        printf("Wrote %zu bytes\n", written);
        
        Dmod_FileClose(file);
        printf("Closed file\n");
        
        // Try to read it back
        file = Dmod_FileOpen("/ram/test.txt", "r");
        if (file) {
            char buffer[256];
            size_t read = Dmod_FileRead(buffer, 1, sizeof(buffer) - 1, file);
            buffer[read] = '\0';
            printf("Read back: \"%s\"\n", buffer);
            
            size_t size = Dmod_FileSize(file);
            printf("File size: %zu bytes\n", size);
            
            Dmod_FileClose(file);
        }
    } else {
        printf("Note: Could not open file (no file system mounted or module not loaded)\n");
    }
    printf("\n");

    // 5. Check file availability
    printf("Checking file availability...\n");
    if (Dmod_FileAvailable("/ram/test.txt")) {
        printf("/ram/test.txt is available\n");
    } else {
        printf("/ram/test.txt is not available\n");
    }
    printf("\n");

    // 6. Unmount file systems
    printf("Unmounting file systems...\n");
    if (DmodVfs_IsMounted("/ram")) {
        DmodVfs_Unmount("/ram");
        printf("Unmounted /ram\n");
    }
    if (DmodVfs_IsMounted("/sd")) {
        DmodVfs_Unmount("/sd");
        printf("Unmounted /sd\n");
    }
    printf("\n");

    // 7. Cleanup
    printf("Deinitializing VFS...\n");
    DmodVfs_Deinit();
    printf("VFS deinitialized\n\n");

    printf("=== Example completed ===\n");
    return 0;
}
