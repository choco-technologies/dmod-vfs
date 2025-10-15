# dmod-vfs - Virtual File System for DMOD

dMOD VFS is a lightweight virtual file system library designed for the [DMOD (Dynamic Modules)](https://github.com/choco-technologies/dmod) ecosystem. It provides a unified interface for managing multiple file systems that implement the [FSI (File System Interface)](https://github.com/choco-technologies/dmod-fsi).

## Features

- **Virtual File System**: Mount multiple file systems at different mount points
- **FSI Integration**: Works with any file system module implementing the FSI interface
- **DMOD File API**: Implements the required file operations for DMOD system side
- **Minimal Dependencies**: Only requires DMOD and string.h (no stdlib)
- **Low Memory Footprint**: Designed for embedded systems with limited resources
- **Path-based Routing**: Automatically routes file operations to the correct file system

## Architecture

The DMOD VFS sits between DMOD and file system implementations:

```
┌─────────────────┐
│   DMOD Boot     │
│   (System)      │
└────────┬────────┘
         │ Uses Dmod_FileOpen, etc.
         │
┌────────▼────────┐
│   DMOD VFS      │
│  (This Library) │
└────────┬────────┘
         │ Routes to mounted FS
         │
┌────────▼────────┬───────────────┬──────────────┐
│   RamFS Module  │ FatFS Module  │ Flash Module │
│  (Implements    │ (Implements   │ (Implements  │
│   FSI DIF)      │   FSI DIF)    │   FSI DIF)   │
└─────────────────┴───────────────┴──────────────┘
```

## Building

### Prerequisites

- DMOD library: https://github.com/choco-technologies/dmod
- DMOD-FSI interface: https://github.com/choco-technologies/dmod-fsi
- GCC or compatible C compiler
- Make or CMake

### Build with Make

```bash
# Set DMOD_DIR to point to your DMOD installation
export DMOD_DIR=/path/to/dmod

# Build the library
make

# The static library will be created at: build/lib/libdmod-vfs.a
```

### Build with CMake

```bash
# Create build directory
mkdir build && cd build

# Configure with DMOD path
cmake -DDMOD_DIR=/path/to/dmod ..

# Build
cmake --build .

# Install (optional)
sudo cmake --install .
```

## Usage

### 1. Initialize the VFS

```c
#include "dmod_vfs.h"

// Initialize VFS system
if (!DmodVfs_Init()) {
    // Handle error
}
```

### 2. Mount File Systems

```c
// Mount a RAM file system at /ram
DmodVfs_Mount("/ram", "ramfs");

// Mount a FatFS file system at /sd
DmodVfs_Mount("/sd", "fatfs");

// Mount a flash file system at /flash
DmodVfs_Mount("/flash", "flashfs");
```

### 3. Use DMOD File API

Once mounted, you can use the DMOD file operations which are automatically routed through the VFS:

```c
// Open a file on the RAM file system
void* file = Dmod_FileOpen("/ram/test.txt", "w");
if (file) {
    const char* data = "Hello, VFS!";
    Dmod_FileWrite(data, 1, strlen(data), file);
    Dmod_FileClose(file);
}

// Open a file on the SD card
file = Dmod_FileOpen("/sd/config.ini", "r");
if (file) {
    char buffer[256];
    size_t bytesRead = Dmod_FileRead(buffer, 1, sizeof(buffer), file);
    // Process data...
    Dmod_FileClose(file);
}
```

### 4. Check File Availability

```c
if (Dmod_FileAvailable("/ram/test.txt")) {
    // File exists
}
```

### 5. Unmount and Cleanup

```c
// Unmount file systems
DmodVfs_Unmount("/ram");
DmodVfs_Unmount("/sd");

// Deinitialize VFS
DmodVfs_Deinit();
```

## API Reference

### Initialization

- `bool DmodVfs_Init(void)` - Initialize the VFS system
- `void DmodVfs_Deinit(void)` - Deinitialize and cleanup

### Mount Management

- `bool DmodVfs_Mount(const char* mountPoint, const char* fsName)` - Mount a file system
- `bool DmodVfs_Unmount(const char* mountPoint)` - Unmount a file system
- `bool DmodVfs_IsMounted(const char* path)` - Check if a path is mounted

### File Operations

- `void* DmodVfs_FileOpen(const char* path, const char* mode)` - Open a file
- `size_t DmodVfs_FileRead(void* buffer, size_t size, size_t count, void* file)` - Read from file
- `size_t DmodVfs_FileWrite(const void* buffer, size_t size, size_t count, void* file)` - Write to file
- `int DmodVfs_FileSeek(void* file, long offset, int origin)` - Seek in file
- `size_t DmodVfs_FileTell(void* file)` - Get current position
- `size_t DmodVfs_FileSize(void* file)` - Get file size
- `void DmodVfs_FileClose(void* file)` - Close file
- `bool DmodVfs_FileAvailable(const char* path)` - Check if file exists

### DMOD System Integration

The library also provides implementations of the DMOD file API functions:

- `void* Dmod_FileOpen(const char* Path, const char* Mode)`
- `size_t Dmod_FileRead(void* Buffer, size_t Size, size_t Count, void* File)`
- `size_t Dmod_FileWrite(const void* Buffer, size_t Size, size_t Count, void* File)`
- `int Dmod_FileSeek(void* File, long Offset, int Origin)`
- `size_t Dmod_FileTell(void* File)`
- `size_t Dmod_FileSize(void* File)`
- `void Dmod_FileClose(void* File)`
- `bool Dmod_FileAvailable(const char* Path)`

## Configuration

You can configure the VFS by defining these macros before including the header:

```c
#define DMOD_VFS_MAX_MOUNT_POINTS 8      // Maximum number of mount points (default: 8)
#define DMOD_VFS_MAX_PATH_LENGTH 256     // Maximum path length (default: 256)

#include "dmod_vfs.h"
```

## Integration with dmod-boot

To integrate dmod-vfs with dmod-boot:

1. Build dmod-vfs as a static library
2. Link the static library with dmod-boot
3. Initialize VFS early in the boot process
4. Mount required file systems
5. DMOD will automatically use VFS for file operations

Example linker flags:
```
-ldmod-vfs -L/path/to/dmod-vfs/build/lib
```

## File System Module Requirements

File system modules must implement the FSI (File System Interface) DIF. See the [dmod-fsi](https://github.com/choco-technologies/dmod-fsi) repository for the interface definition and examples.

Key requirements:
- Implement FSI DIF functions (fopen, fclose, fread, fwrite, lseek, tell, size, stat)
- Build as DMOD module (.dmf file)
- Use `dmod_fsi_dif_api_declaration` macro for implementations

## Examples

See the `examples/` directory for:
- Basic VFS usage
- Integration with different file systems
- DMOD boot integration

## Coding Style

This library follows the DMOD coding style:
- CamelCase for types and public API functions
- Prefix all public APIs with `DmodVfs_`
- Use DMOD logging macros (DMOD_LOG_INFO, DMOD_LOG_ERROR)
- Use DMOD memory functions (Dmod_Malloc, Dmod_Free)

## License

MIT License - See LICENSE file for details

## Contributing

Contributions are welcome! Please ensure:
- Code follows the DMOD coding style
- Changes are minimal and focused
- Documentation is updated
- No additional dependencies are introduced

## References

- DMOD: https://github.com/choco-technologies/dmod
- DMOD-FSI: https://github.com/choco-technologies/dmod-fsi
- DMOD-Boot: https://github.com/choco-technologies/dmod-boot
