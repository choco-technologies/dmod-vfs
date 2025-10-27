# dmod-vfs
dMOD VFS - Virtual File System

## Overview
DMVFS is a virtual file system layer that provides a unified interface for multiple file system implementations through the DMOD (Dynamic Module) system.

## Building

```bash
mkdir build && cd build
cmake ..
make
```

## Testing

The project includes an integration test suite that tests dmvfs with a real filesystem implementation (RamFS). Tests are built by default. To build and run tests:

```bash
cd build
cmake ..
make
ctest --output-on-failure
```

To disable tests, use `-DBUILD_TESTS=OFF` when running cmake.

See [tests/README.md](tests/README.md) for detailed information about the test suite.