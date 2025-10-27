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

The project includes a comprehensive test suite covering all interface functions. To build and run tests:

```bash
cd build
cmake -DBUILD_TESTS=ON ..
make
ctest --output-on-failure
```

See [tests/README.md](tests/README.md) for detailed information about the test suite.