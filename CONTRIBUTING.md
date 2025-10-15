# Contributing to DMOD VFS

Thank you for your interest in contributing to DMOD VFS! This document provides guidelines for contributing to the project.

## Development Setup

### Prerequisites

1. **DMOD Library**: Clone and build the DMOD library
   ```bash
   git clone https://github.com/choco-technologies/dmod.git
   cd dmod
   make
   ```

2. **DMOD-FSI Interface** (optional, for testing)
   ```bash
   git clone https://github.com/choco-technologies/dmod-fsi.git
   cd dmod-fsi
   make
   ```

3. **Build Tools**
   - GCC or compatible C compiler
   - Make
   - CMake (optional)

### Building

```bash
# Clone the repository
git clone https://github.com/choco-technologies/dmod-vfs.git
cd dmod-vfs

# Build with Make
make DMOD_DIR=/path/to/dmod

# Or build with CMake
mkdir build && cd build
cmake -DDMOD_DIR=/path/to/dmod ..
make
```

## Coding Standards

### Style Guide

This project follows the DMOD coding style:

1. **Naming Conventions**
   - Types: `CamelCase` with `_t` suffix (e.g., `DmodVfs_MountPoint_t`)
   - Public functions: `CamelCase` with module prefix (e.g., `DmodVfs_Mount`)
   - Static functions: `CamelCase` without prefix (e.g., `FindMountPoint`)
   - Constants: `UPPER_CASE` (e.g., `DMOD_VFS_MAX_MOUNT_POINTS`)
   - Variables: `camelCase` (e.g., `mountPoint`, `fsName`)

2. **Formatting**
   - Indentation: 4 spaces (no tabs)
   - Braces: Opening brace on same line
   - Line length: Prefer 80-100 characters, max 120
   - Comments: Use `//` for single-line, `/* */` for multi-line

3. **File Organization**
   - Copyright header at top of each file
   - Includes grouped: system headers, DMOD headers, local headers
   - Group related functions together
   - Separate sections with comment dividers

### Example Code

```c
/**
 * @brief Find mount point for a given path
 */
static DmodVfs_MountPoint_t* FindMountPoint(const char* path)
{
    if (!path) {
        return NULL;
    }

    size_t bestMatchLen = 0;
    DmodVfs_MountPoint_t* bestMatch = NULL;

    for (int i = 0; i < DMOD_VFS_MAX_MOUNT_POINTS; i++) {
        if (!s_mountPoints[i].active) {
            continue;
        }
        // ... implementation ...
    }

    return bestMatch;
}
```

## Making Changes

### Branch Naming

- Feature branches: `feature/description`
- Bug fixes: `fix/description`
- Documentation: `docs/description`

### Commit Messages

Follow conventional commit format:

```
type(scope): brief description

Detailed description if needed.

- List of changes
- Another change
```

Types: `feat`, `fix`, `docs`, `style`, `refactor`, `test`, `chore`

Examples:
```
feat(mount): add support for mount options
fix(vfs): resolve path matching issue with trailing slashes
docs(readme): update integration guide
```

### Pull Request Process

1. **Create a feature branch**
   ```bash
   git checkout -b feature/my-feature
   ```

2. **Make your changes**
   - Follow coding standards
   - Add tests if applicable
   - Update documentation

3. **Build and test**
   ```bash
   make clean
   make DMOD_DIR=/path/to/dmod
   # Run any tests
   ```

4. **Commit your changes**
   ```bash
   git add .
   git commit -m "feat(scope): description"
   ```

5. **Push and create PR**
   ```bash
   git push origin feature/my-feature
   ```

6. **PR Requirements**
   - Clear description of changes
   - Reference related issues
   - Pass all checks
   - Maintain backward compatibility

## Code Review Guidelines

### For Contributors

- Respond to feedback promptly
- Be open to suggestions
- Explain your reasoning
- Update PR based on feedback

### For Reviewers

- Be constructive and respectful
- Focus on code quality and maintainability
- Check for edge cases
- Verify documentation updates

## Testing

### Manual Testing

1. **Build the library**
   ```bash
   make clean && make DMOD_DIR=/path/to/dmod
   ```

2. **Check for warnings**
   ```bash
   # Should have no warnings
   make 2>&1 | grep warning
   ```

3. **Test with example**
   ```bash
   cd examples/basic
   make
   ./example
   ```

### Test Checklist

- [ ] Library builds without warnings
- [ ] All public APIs work as documented
- [ ] Error cases are handled gracefully
- [ ] Memory leaks checked (if possible)
- [ ] Examples compile and run
- [ ] Documentation is up to date

## Documentation

### Documentation Standards

1. **Code Documentation**
   - All public functions must have Doxygen comments
   - Include parameter descriptions and return values
   - Add usage examples for complex functions

2. **File Documentation**
   - Each header file should have a file-level comment
   - Describe the purpose of the file
   - List any dependencies

3. **README Updates**
   - Update README.md for new features
   - Add examples for new APIs
   - Update troubleshooting if applicable

4. **Architecture Documentation**
   - Update ARCHITECTURE.md for design changes
   - Document design decisions
   - Explain trade-offs

### Example Documentation

```c
/**
 * @brief Mount a file system at a given mount point
 * 
 * This function mounts a file system module implementing the FSI interface
 * at the specified mount point. The file system module must be loaded via
 * DMOD before calling this function.
 * 
 * @param mountPoint Mount point path (e.g., "/dev", "/ram")
 * @param fsName Name of the file system module to mount
 * @return true on success, false otherwise
 * 
 * @note The mount point path should not end with a trailing slash
 * @note Multiple file systems can be mounted at different paths
 * 
 * Example:
 * @code
 * if (DmodVfs_Mount("/ram", "ramfs")) {
 *     // File system mounted successfully
 * }
 * @endcode
 */
bool DmodVfs_Mount(const char* mountPoint, const char* fsName);
```

## Issue Reporting

### Bug Reports

Please include:
- Clear description of the issue
- Steps to reproduce
- Expected vs actual behavior
- Environment details (OS, compiler, DMOD version)
- Relevant logs or error messages

### Feature Requests

Please include:
- Description of the feature
- Use case / motivation
- Proposed API or implementation (if any)
- Impact on existing functionality

## License

By contributing to this project, you agree that your contributions will be licensed under the MIT License.

## Questions?

If you have questions about contributing:
- Check existing issues and discussions
- Review the documentation (README, ARCHITECTURE)
- Open a new issue for clarification

Thank you for contributing to DMOD VFS!
