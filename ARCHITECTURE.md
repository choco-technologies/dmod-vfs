# DMOD VFS Architecture

This document describes the architecture and design decisions of the DMOD VFS (Virtual File System).

## Overview

The DMOD VFS is a lightweight virtual file system layer designed to work with the DMOD dynamic module system. It provides:

1. **Unified File API**: A single interface for all file operations regardless of underlying file system
2. **Multiple File Systems**: Support for mounting multiple file systems at different paths
3. **Dynamic Discovery**: Automatic discovery of file system modules implementing the FSI interface
4. **Minimal Overhead**: Lightweight implementation suitable for embedded systems

## Architecture Diagram

```
┌─────────────────────────────────────────────────────────────┐
│  Application / DMOD Modules                                  │
│  (Uses Dmod_FileOpen, Dmod_FileRead, etc.)                  │
└────────────────────────┬────────────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────┐
│  DMOD VFS (libdmod-vfs.a)                                    │
│  ┌────────────────────────────────────────────────────────┐ │
│  │  VFS Core                                              │ │
│  │  - Mount point management                              │ │
│  │  - Path resolution                                     │ │
│  │  - File handle management                              │ │
│  └────────────────────┬───────────────────────────────────┘ │
│                       │                                      │
│  ┌────────────────────▼───────────────────────────────────┐ │
│  │  DIF Integration Layer                                 │ │
│  │  - FSI module discovery (Dmod_GetNextDifModule)        │ │
│  │  - Function pointer resolution (Dmod_GetDifFunction)   │ │
│  │  - Weak FSI symbols                                    │ │
│  └────────────────────┬───────────────────────────────────┘ │
└───────────────────────┼─────────────────────────────────────┘
                        │
                        │ DIF (DMOD Interface)
                        │
          ┌─────────────┼─────────────┬──────────────┐
          │             │             │              │
          ▼             ▼             ▼              ▼
┌──────────────┐ ┌──────────────┐ ┌────────────┐ ┌────────────┐
│  RamFS       │ │  FatFS       │ │  FlashFS   │ │  DevFS     │
│  (.dmf)      │ │  (.dmf)      │ │  (.dmf)    │ │  (.dmf)    │
│              │ │              │ │            │ │            │
│ Implements   │ │ Implements   │ │ Implements │ │ Implements │
│ FSI DIF      │ │ FSI DIF      │ │ FSI DIF    │ │ FSI DIF    │
└──────────────┘ └──────────────┘ └────────────┘ └────────────┘
```

## Key Components

### 1. VFS Core

**File**: `src/dmod_vfs.c`

The VFS core manages:
- Mount point table (array of mount points)
- Path resolution (finding which mount point handles a path)
- File handle wrapping (adding VFS metadata to FS handles)

**Key Structures**:
```c
// Mount point entry
typedef struct DmodVfs_MountPoint_s {
    char mountPoint[DMOD_VFS_MAX_PATH_LENGTH];
    char fsName[64];
    Dmod_Context_t* fsContext;
    DmodVfs_FsOps_t ops;  // Function pointers
    bool active;
} DmodVfs_MountPoint_t;

// File handle wrapper
typedef struct DmodVfs_FileHandle_s {
    void* fsHandle;  // Actual FS handle
    DmodVfs_MountPoint_t* mountPoint;
} DmodVfs_FileHandle_t;
```

### 2. DMOD SAL Implementation

**File**: `src/dmod_vfs_sal.c`

Implements the DMOD System Abstraction Layer (SAL) file functions:
- `Dmod_FileOpen()`, `Dmod_FileRead()`, etc.
- These functions simply delegate to the VFS core functions

### 3. FSI Weak Symbols

**File**: `src/dmod_vfs_fsi_weak.c`

Provides weak symbol declarations for FSI DIF signatures:
- Allows VFS to compile without FSI module
- Runtime detection of FSI availability
- Graceful degradation if modules not available

### 4. Public API

**File**: `inc/dmod_vfs.h`

Public API provides:
- VFS management: `DmodVfs_Init()`, `DmodVfs_Deinit()`
- Mount management: `DmodVfs_Mount()`, `DmodVfs_Unmount()`
- File operations: `DmodVfs_FileOpen()`, `DmodVfs_FileRead()`, etc.

## Data Flow

### Mount Operation

```
1. Application calls DmodVfs_Mount("/ram", "ramfs")
2. VFS finds free mount point slot
3. VFS iterates through FSI modules using Dmod_GetNextDifModule(dmod_fsi_fopen_sig, ...)
4. For each module, VFS calls Dmod_Context_GetModuleName() to get module name
5. VFS compares module name with requested name ("ramfs")
6. When match is found, VFS gets function pointers via Dmod_GetDifFunction()
7. VFS stores mount point with function pointers
8. Mount complete
```

### File Open Operation

```
1. Application calls Dmod_FileOpen("/ram/test.txt", "r")
2. SAL redirects to DmodVfs_FileOpen()
3. VFS finds mount point matching "/ram"
4. VFS gets relative path ("test.txt")
5. VFS calls mount point's fopen() function
6. File system returns its file handle
7. VFS wraps handle with VFS metadata
8. Returns VFS handle to application
```

### File Read Operation

```
1. Application calls Dmod_FileRead(buffer, size, count, handle)
2. SAL redirects to DmodVfs_FileRead()
3. VFS unwraps handle to get FS handle and mount point
4. VFS calls mount point's fread() function
5. File system performs actual read
6. Returns bytes read to VFS
7. VFS converts to element count and returns
```

## Design Decisions

### Why Use DIF Instead of Direct Linking?

**Decision**: Use DMOD's DIF (Dynamic Interface) system for file system discovery.

**Rationale**:
- Allows dynamic loading/unloading of file systems
- No compile-time dependencies between VFS and specific file systems
- Multiple file systems can be used simultaneously
- Follows DMOD design philosophy of modularity

**Alternatives Considered**:
- Static linking: Would require recompilation for different file systems
- Function table registration: Would require explicit registration calls

### Why Wrap File Handles?

**Decision**: VFS creates wrapper handles instead of passing FS handles directly.

**Rationale**:
- VFS needs to know which mount point a handle belongs to for operations
- Allows VFS to add metadata without modifying file systems
- Enables future extensions (e.g., handle validation, statistics)

**Trade-offs**:
- Small memory overhead per file (~16 bytes)
- Extra allocation/deallocation operations
- Minimal performance impact

### Why Use Weak FSI Symbols?

**Decision**: Declare FSI DIF signatures as weak symbols.

**Rationale**:
- VFS can compile and link without FSI module present
- Useful for testing and development
- Provides runtime detection and graceful error messages
- Follows principle of minimal dependencies

**Trade-offs**:
- Slightly more complex build process
- Requires careful documentation
- Runtime errors instead of link-time errors

### Why No File Handle Caching?

**Decision**: Don't cache file handles or paths in VFS layer.

**Rationale**:
- Keeps VFS simple and minimal
- File systems can implement their own caching strategies
- Embedded systems often have limited memory for caches
- Application can cache handles at its level if needed

**Trade-offs**:
- Slightly more work for repeated path resolutions
- No centralized cache statistics
- File systems must handle caching individually

## Memory Usage

### Static Memory

- Mount point table: `DMOD_VFS_MAX_MOUNT_POINTS * ~300 bytes`
- Default (8 mount points): ~2.4 KB

### Dynamic Memory

- Per file handle: 16 bytes (VFS wrapper structure)
- File system handles: Varies by file system

### Stack Usage

- Most VFS functions use <256 bytes of stack
- Path operations may use up to `DMOD_VFS_MAX_PATH_LENGTH` (256 bytes default)

## Thread Safety

**Current Implementation**: Not thread-safe.

**Rationale**:
- Embedded systems often use simple scheduler or no RTOS
- Thread safety adds overhead
- DMOD provides mutex APIs if needed

**Future Enhancement**: Could add critical sections around mount point access.

## Error Handling

**Philosophy**: Fail gracefully with informative logging.

**Approach**:
1. Check parameters for NULL/invalid values
2. Log errors with DMOD_LOG_ERROR
3. Return appropriate error codes (NULL, 0, -1, false)
4. Don't crash or assert (suitable for embedded systems)

**Example**:
```c
if (!s_initialized) {
    DMOD_LOG_ERROR("VFS not initialized\n");
    return false;
}
```

## Testing Strategy

### Unit Testing

- Test mount/unmount operations
- Test path resolution (longest prefix matching)
- Test handle management
- Test error conditions

### Integration Testing

- Test with actual FSI modules (RamFS, etc.)
- Test multiple concurrent file systems
- Test file operations across mount points
- Test edge cases (max mounts, path lengths)

### System Testing

- Integration with dmod-boot
- Performance testing
- Memory leak testing
- Stress testing (many files, operations)

## Future Enhancements

### Possible Improvements

1. **Mount Options**: Support for read-only mounts, mount flags
2. **Path Caching**: Cache last-used mount point for performance
3. **Directory Operations**: Add comprehensive directory support
4. **Link Support**: Symbolic links, hard links
5. **Access Control**: Permission checking at VFS layer
6. **Statistics**: Track operation counts, performance metrics
7. **Thread Safety**: Add mutex protection for multi-threaded systems

### Backward Compatibility

All enhancements should maintain backward compatibility with the current API.

## References

- [DMOD Documentation](https://github.com/choco-technologies/dmod)
- [DMOD-FSI Interface](https://github.com/choco-technologies/dmod-fsi)
- [DMOD-Boot](https://github.com/choco-technologies/dmod-boot)
- [POSIX File System API](https://pubs.opengroup.org/onlinepubs/9699919799/)
