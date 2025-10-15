# Example: Basic VFS Usage

This example demonstrates how to use the dmod-vfs library.

## Overview

The example shows:
- Initializing the VFS
- Mounting file systems
- Using DMOD file API functions
- Unmounting and cleanup

## Files

- `main.c` - Example code showing VFS usage
- `Makefile` - Build configuration

## Building

```bash
# Make sure DMOD is built first
cd /path/to/dmod
make

# Build this example
cd /path/to/dmod-vfs/examples/basic
make DMOD_DIR=/path/to/dmod
```

## Running

```bash
./example
```

## Code Structure

```c
// 1. Initialize VFS
DmodVfs_Init();

// 2. Mount file systems
DmodVfs_Mount("/ram", "ramfs");

// 3. Use DMOD file operations
void* file = Dmod_FileOpen("/ram/test.txt", "w");
Dmod_FileWrite("Hello", 1, 5, file);
Dmod_FileClose(file);

// 4. Cleanup
DmodVfs_Unmount("/ram");
DmodVfs_Deinit();
```

## Notes

- This example requires file system modules (e.g., ramfs) to be loaded
- The VFS automatically routes file operations to the correct file system
- All DMOD file API functions (Dmod_FileOpen, etc.) work through the VFS
