# DMVFS Tests

This directory contains integration tests for the DMVFS (DMOD Virtual File System) interface using a real filesystem implementation.

## Test Structure

The tests use the RamFS (RAM-based filesystem) module from the dmfsi examples to perform integration testing of the dmvfs API with actual filesystem operations.

### Integration Test (`test_dmvfs_integration.c`)
Comprehensive integration test that:
1. Initializes the DMOD system
2. Initializes DMVFS
3. Mounts the RamFS filesystem
4. Tests all dmvfs operations with real file I/O

**Test Coverage:**
- **Initialization**: DMVFS init/deinit, parameter validation
- **Mounting**: Mount/unmount RamFS filesystem
- **File Operations**: 
  - Create, open, close files
  - Read and write data
  - Seek and tell operations
  - File removal (unlink)
- **Directory Operations**: 
  - Create and remove directories
  - Check directory existence
- **Path Operations**: 
  - Get current working directory
  - Convert relative to absolute paths
- **Multiple Files**: Handle multiple open files simultaneously

### RamFS Module (`ramfs/`)
The RamFS module is a simple RAM-based filesystem implementation that:
- Stores files entirely in memory
- Implements the full DMFSI interface
- Provides a real filesystem for integration testing

## Building the Tests

Tests are built automatically when the `BUILD_TESTS` CMake option is enabled (default: ON):

```bash
cd build
cmake ..
make
```

This will:
1. Build the RamFS filesystem module
2. Build the DMVFS library
3. Build the integration test executable

## Running the Tests

### Run with CTest:
```bash
cd build
ctest --output-on-failure
```

### Run directly:
```bash
cd build/tests
./test_dmvfs_integration
```

### Run with make:
```bash
cd build
make run_tests
```

## Test Framework

The tests use a lightweight custom test framework defined in `test_framework.h` with:

- `TEST_ASSERT(condition, message)` - Assert a boolean condition
- `TEST_ASSERT_EQUAL(expected, actual, message)` - Assert equality
- `TEST_ASSERT_NOT_NULL(ptr, message)` - Assert pointer is not NULL
- `TEST_ASSERT_STR_EQUAL(expected, actual, message)` - Assert string equality
- `RUN_TEST(test_func)` - Run a test function
- `TEST_SUMMARY()` - Print test results summary
- `TEST_RETURN_CODE()` - Return exit code based on test results

## Test Approach

Unlike unit tests, these are **integration tests** that:
- Use a real filesystem implementation (RamFS)
- Initialize the full DMOD system
- Test actual file operations end-to-end
- Verify filesystem behavior with real I/O

This approach validates that dmvfs correctly integrates with filesystem modules and handles real-world operations.

## Requirements

- DMOD system
- DMFSI interface
- RamFS filesystem module
- CMake 3.10+
- C compiler with C99 support

## Expected Output

When tests pass, you'll see output like:
```
========================================
DMVFS Integration Tests with RamFS
========================================

Running: test_dmvfs_init_deinit
[PASS] Initialize DMVFS with valid parameters
[PASS] Get max mount points
...

========================================
Test Summary:
  Total:  XX
  Passed: XX
  Failed: 0
========================================
```

