# DMVFS Tests

This directory contains comprehensive unit tests for the DMVFS (DMOD Virtual File System) interface.

## Test Structure

The tests are organized into several test suites:

### 1. Initialization Tests (`test_init.c`)
Tests for DMVFS initialization and deinitialization:
- Valid and invalid parameter handling
- Double initialization prevention
- Getting max mount points and open files
- State before initialization

### 2. Path Operations Tests (`test_path.c`)
Tests for path-related operations:
- `getcwd` - Get current working directory
- `getpwd` - Get process working directory  
- `toabs` - Convert relative paths to absolute paths
- Buffer size handling and NULL pointer checks

### 3. File Operations Tests (`test_file_ops.c`)
Tests for file I/O operations:
- `fopen` / `fclose` - File opening and closing
- `fread` / `fwrite` - File reading and writing
- `fseek` / `ftell` / `feof` - File positioning
- Process-specific file closing
- NULL pointer and invalid parameter handling

### 4. Mount Operations Tests (`test_mount.c`)
Tests for filesystem mounting:
- `mount_fs` - Mount filesystem at a path
- `unmount_fs` - Unmount filesystem
- Non-existent filesystem handling
- NULL parameter validation

### 5. Directory Operations Tests (`test_dir_ops.c`)
Tests for directory operations:
- `mkdir` / `rmdir` - Directory creation and removal
- `chdir` - Change current directory
- `opendir` / `readdir` / `closedir` - Directory iteration
- `direxists` - Directory existence checking
- NULL pointer and invalid parameter handling

## Building the Tests

Tests are built automatically when the `BUILD_TESTS` CMake option is enabled (default: ON):

```bash
cd build
cmake ..
make
```

This will build all test executables in the `build/tests/` directory.

## Running the Tests

### Run all tests with CTest:
```bash
cd build
ctest --output-on-failure
```

### Run individual test suites:
```bash
cd build/tests
./test_init
./test_path
./test_file_ops
./test_mount
./test_dir_ops
```

### Run tests with make:
```bash
cd build
make run_tests
```

## Test Framework

The tests use a lightweight custom test framework defined in `test_framework.h` with the following macros:

- `TEST_ASSERT(condition, message)` - Assert a boolean condition
- `TEST_ASSERT_EQUAL(expected, actual, message)` - Assert equality
- `TEST_ASSERT_NOT_NULL(ptr, message)` - Assert pointer is not NULL
- `TEST_ASSERT_NULL(ptr, message)` - Assert pointer is NULL
- `TEST_ASSERT_STR_EQUAL(expected, actual, message)` - Assert string equality
- `RUN_TEST(test_func)` - Run a test function
- `TEST_SUMMARY()` - Print test results summary
- `TEST_RETURN_CODE()` - Return exit code based on test results

## Test Coverage

The tests cover:
- ✅ API parameter validation (NULL pointers, invalid values)
- ✅ State management (before/after initialization)
- ✅ Error handling and return codes
- ✅ Boundary conditions (buffer sizes, resource limits)
- ✅ Concurrent operations prevention (double init)

## Mock Filesystem

The tests include a minimal mock filesystem implementation (`mock_fs_sigs.c`) that provides the DIF (DMOD Interface) signatures required by DMVFS. This allows the tests to link successfully without requiring a full filesystem implementation.

## Current Test Results

As of the latest run:
- **Total Tests**: 90+ assertions across 5 test suites
- **Pass Rate**: ~98% (88/90 assertions passing)
- **Known Issues**: 
  - Minor path formatting issue with double slashes
  - Global state not fully reset between test executables

## Future Improvements

Potential enhancements for the test suite:
- Integration tests with a real mock filesystem implementation
- Performance/stress tests with many files and mount points  
- Thread safety tests
- Memory leak detection with valgrind
- Code coverage analysis
