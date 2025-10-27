# DMVFS Test Suite

This directory contains a comprehensive test suite for the DMVFS (DMOD Virtual File System) API.

## Structure

The test suite is organized into multiple files for better maintainability:

- `main.c` - Entry point that initializes DMVFS, mounts the filesystem, and runs all tests
- `test_framework.h/c` - Simple test framework with assertion macros and result tracking
- `test_file_operations.c` - Tests for file operations (open, read, write, seek, close, etc.)
- `test_directory_operations.c` - Tests for directory operations (mkdir, rmdir, opendir, readdir, etc.)
- `test_path_operations.c` - Tests for path operations (getcwd, chdir, toabs, etc.)

## Building

The test suite is built automatically with the dmvfs project:

```bash
mkdir build && cd build
cmake ..
make
```

The executable will be located at `build/tests/dmvfs_example`.

## Running Tests

To run the tests, you need to provide a ramfs DMF module file:

```bash
./dmvfs_example path/to/ramfs.dmf
```

### Building ramfs Module

The ramfs module is part of the dmfsi project. To build it in MODULE mode (to generate a .dmf file):

1. Clone the dmfsi repository
2. Build in DMOD_MODULE mode
3. The ramfs.dmf file will be generated in the build directory

Alternatively, you can build ramfs using the dmfsi Makefile in the examples/ramfs directory.

## Adding New Tests

To add new tests to the suite:

1. Create a new test file (e.g., `test_new_feature.c`)
2. Include `test_framework.h` and `dmvfs.h`
3. Implement test functions with the signature: `static void test_function_name(void)`
4. Use the TEST_ASSERT macros to verify expected behavior:
   - `TEST_ASSERT(condition, message)` - Assert that condition is true
   - `TEST_ASSERT_EQ(actual, expected, message)` - Assert equality
   - `TEST_ASSERT_NEQ(actual, not_expected, message)` - Assert inequality
   - `TEST_ASSERT_STR_EQ(actual, expected, message)` - Assert string equality
5. Create a test array at the end of your file:
   ```c
   test_case_t my_new_tests[] = {
       {"Test Name 1", test_function_1},
       {"Test Name 2", test_function_2},
       {NULL, NULL}  // Sentinel
   };
   ```
6. Add the extern declaration to `test_framework.h`
7. Add the test execution loop to `RunAllTests()` in `main.c`
8. Update `CMakeLists.txt` to include your new test file

## Test Coverage

The current test suite covers:

### File Operations
- File creation and writing
- File reading
- File seeking (SEEK_SET, SEEK_CUR, SEEK_END)
- File appending
- File truncation
- File statistics
- File removal
- File renaming
- Character I/O (getc/putc)

### Directory Operations
- Directory creation
- Directory statistics
- Creating files in directories
- Reading directory contents
- Nested directories
- Directory removal

### Path Operations
- Current working directory (getcwd/chdir)
- Relative path operations
- Absolute path conversion (toabs)
- Present working directory (getpwd)
- Nested path operations

## Exit Codes

- `0` - All tests passed
- `1` - One or more tests failed
- `-1` - Test suite initialization failed
