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

1. **Build dmod-vfs as a static library**
   ```bash
   cd /path/to/dmod-vfs
   make DMOD_DIR=/path/to/dmod
   # Output: build/lib/libdmod-vfs.a
   ```

2. **Link the static library with dmod-boot**
   
   In your dmod-boot Makefile or CMakeLists.txt:
   ```makefile
   # Makefile
   LIBS += -ldmod-vfs -L/path/to/dmod-vfs/build/lib
   CFLAGS += -I/path/to/dmod-vfs/inc
   ```
   
   Or in CMakeLists.txt:
   ```cmake
   # CMakeLists.txt
   include_directories(/path/to/dmod-vfs/inc)
   link_directories(/path/to/dmod-vfs/build/lib)
   target_link_libraries(dmod-boot dmod-vfs)
   ```

3. **Initialize VFS early in the boot process**
   ```c
   #include "dmod_vfs.h"
   
   void system_init(void) {
       // Initialize VFS before loading modules
       if (!DmodVfs_Init()) {
           // Handle error
       }
   }
   ```

4. **Mount required file systems**
   ```c
   void system_mount_filesystems(void) {
       // Mount file systems after loading FSI modules
       DmodVfs_Mount("/dev", "devfs");
       DmodVfs_Mount("/ram", "ramfs");
       DmodVfs_Mount("/flash", "flashfs");
   }
   ```

5. **Use DMOD file operations normally**
   
   After mounting, all DMOD file operations will automatically use the VFS:
   ```c
   // This will be routed to the correct file system
   void* file = Dmod_FileOpen("/ram/config.txt", "r");
   // ... use file ...
   Dmod_FileClose(file);
   ```

### Linker Considerations

When linking with dmod-boot, make sure the VFS library is linked **before** any modules that might provide the DMOD functions (malloc, printf, etc.). The typical link order should be:

```
dmod-boot.o ... -ldmod-vfs -ldmod -lc
```

### Memory Requirements

The VFS has minimal memory requirements:
- Static memory: ~2KB (for mount point table)
- Per-file overhead: ~16 bytes (file handle structure)
- Dynamic memory: Only for file handles (allocated via Dmod_Malloc)

Configure `DMOD_VFS_MAX_MOUNT_POINTS` to adjust memory usage.

## File System Module Requirements

File system modules must implement the FSI (File System Interface) DIF. See the [dmod-fsi](https://github.com/choco-technologies/dmod-fsi) repository for the interface definition and examples.

### Key Requirements

1. **Implement FSI DIF functions**
   - Use `dmod_fsi_dif_api_declaration` macro for implementations
   - Required functions: `fopen`, `fclose`, `fread`, `fwrite`, `lseek`, `tell`, `size`, `stat`
   - Optional functions: `sync`, `eof`, `getc`, `putc`, `fflush`, `error`, directory operations

2. **Build as DMOD module (.dmf file)**
   - Follow DMOD module conventions
   - Use DMOD build system

3. **Module Discovery**
   - The VFS automatically discovers FSI modules using DMOD's DIF API
   - Uses `Dmod_GetNextDifModule()` to find modules implementing FSI
   - Uses `Dmod_GetDifFunction()` to get function pointers

### Example FSI Implementation

```c
// ramfs.c - Example RAM file system module
#define DMOD_ENABLE_REGISTRATION ON
#include "dmod.h"
#include "fsi.h"

// Implement fopen for RamFS
dmod_fsi_dif_api_declaration(1.0, RamFS, int, _fopen, 
    (void** fp, const char* path, int mode, int attr))
{
    // RamFS-specific implementation
    *fp = Dmod_Malloc(sizeof(RamFile));
    // ... initialize file ...
    return FSI_OK;
}

// Implement other FSI functions...
```

### DIF Integration Details

The VFS uses DMOD's DIF (DMOD Interface) system to dynamically discover and use file system modules:

1. **Module Discovery**: When mounting, the VFS searches for modules implementing the FSI DIF using the signature `dmod_fsi_fopen_sig`.

2. **Function Pointer Resolution**: For each found module, the VFS retrieves function pointers for all supported operations using `Dmod_GetDifFunction()`.

3. **Path Routing**: When a file operation is requested, the VFS:
   - Finds the mount point that matches the file path
   - Gets the file system module associated with that mount point
   - Calls the appropriate function from that module

4. **Weak Symbols**: The VFS uses weak symbols for FSI signatures, allowing it to:
   - Compile without the FSI module
   - Detect at runtime if FSI modules are available
   - Provide useful error messages if modules are missing

## Examples

See the `examples/` directory for:
- Basic VFS usage
- Integration with different file systems
- DMOD boot integration

## Troubleshooting

### VFS operations fail with "No mount point found"

**Problem**: File operations return errors indicating no mount point was found.

**Solutions**:
1. Ensure you've called `DmodVfs_Mount()` for the path you're trying to access
2. Check that the mount point matches the beginning of your file path
3. Verify mount was successful (check return value of `DmodVfs_Mount()`)

### Mount fails with "No FSI module found"

**Problem**: `DmodVfs_Mount()` returns false and logs "No FSI module found".

**Solutions**:
1. Ensure file system modules are loaded via DMOD before mounting
2. Verify the FSI module is built correctly and implements the FSI DIF
3. Check that the dmod-fsi interface module is linked
4. Enable verbose logging to see what modules are being searched

### File operations return 0 bytes or fail silently

**Problem**: File operations appear to work but return 0 bytes or have no effect.

**Solutions**:
1. Check that the file system module's functions are actually implemented
2. Verify function pointers were correctly initialized (check VFS logs)
3. Ensure the file system module has been initialized (call its init function if provided)
4. Check file system-specific requirements (e.g., RAM FS needs memory allocation)

### Compilation errors about missing DMOD functions

**Problem**: Linker errors about undefined references to `Dmod_Malloc`, `Dmod_Printf`, etc.

**Solutions**:
1. Link against the DMOD library: `-ldmod`
2. Provide your own implementations of these functions if not using DMOD
3. Ensure dmod-boot or your system provides these functions

### "FSI interface not available" warning

**Problem**: VFS logs "FSI interface not available (fsi.h not linked)".

**Solutions**:
1. This is expected if you're not using FSI modules - VFS will still compile
2. To fix: Link the dmod-fsi interface module or ensure fsi.h is included
3. If intentional (testing without modules), this warning can be ignored

## Performance Considerations

- **Path Resolution**: The VFS uses a simple linear search for mount points. For systems with many mount points, consider optimizing the search.
- **Function Call Overhead**: Each file operation goes through the VFS layer, adding minimal overhead (~2 function calls).
- **Memory Usage**: Each mounted file system uses ~300 bytes. Adjust `DMOD_VFS_MAX_MOUNT_POINTS` based on your needs.
- **No Caching**: The VFS doesn't cache file handles or paths. File systems should implement their own caching if needed.

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
