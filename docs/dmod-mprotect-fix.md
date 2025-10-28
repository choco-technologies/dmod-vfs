# DMOD mprotect Bug Fix

## Issue Description

When loading DMF modules larger than 4KB (one page), the DMOD loader crashes with a segmentation fault during API connection. This affects modules like `testfs` that are approximately 5.9KB in size.

## Root Cause

The bug is in `dmod/src/system/if/dmod_if_mem.c` in the `Dmod_AlignedMalloc` function. The `mprotect` system call is invoked with `pagesize` (4096 bytes) instead of the actual allocated size, resulting in only the first page being marked as readable, writable, and executable (RWX). When the module loader attempts to write to the output API section that resides beyond the first page, it triggers a segmentation fault with `SEGV_ACCERR` (memory access error).

## Technical Details

The original code at line 131:
```c
if (mem != NULL && mprotect(mem, pagesize, PROT_READ | PROT_WRITE | PROT_EXEC) != 0)
```

This sets memory protection for only `pagesize` bytes (4096), but the allocated size `Size` may be larger. For modules larger than one page, the output API section may be located in subsequent pages that lack write permissions.

## Fix

Calculate the correct size to protect by rounding up `Size` to the nearest page boundary:

```c
if (mem != NULL) 
{
    // Round up Size to the nearest page boundary
    size_t prot_size = ((Size + pagesize - 1) / pagesize) * pagesize;
    if (mprotect(mem, prot_size, PROT_READ | PROT_WRITE | PROT_EXEC) != 0) 
    {
        DMOD_LOG_ERROR("Cannot set memory protection. Size: %zu, Pagesize: %zu\n", prot_size, pagesize);
        free(mem);
        return NULL;
    }
}
```

## Status

This fix has been applied to the local copy of dmod in the build directory. However, since dmod is fetched via CMake's FetchContent, the fix needs to be:

1. **Reported to the upstream dmod repository** at https://github.com/choco-technologies/dmod
2. **Applied as a patch** in the dmvfs build process until the upstream fix is available
3. **Pinned to a fixed version** once the upstream repository includes the fix

## Testing

With this fix:
- ✅ `testfs.dmf` (5904 bytes) loads and runs successfully
- ✅ `ramfs.dmf` (5464 bytes) continues to work without regression
- ✅ The preinit function in testfs that uses `Dmod_IsFunctionConnected` executes correctly

## Reproducing the Issue

Without the fix:
```bash
cd build
./tests/dmvfs_example ../tests/testfs/build/dmf/testfs.dmf
# Result: Segmentation fault at address 0x2222f2b0 (SEGV_ACCERR)
```

With the fix:
```bash
cd build
./tests/dmvfs_example ../tests/testfs/build/dmf/testfs.dmf
# Result: Success - module loads, initializes, and runs correctly
```

## Next Steps

1. Create an issue in the dmod repository with this information
2. Submit a pull request to dmod with the fix
3. Consider implementing a patch mechanism in dmvfs's CMakeLists.txt to apply this fix automatically until the upstream fix is merged
