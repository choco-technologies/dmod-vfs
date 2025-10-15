/**
 * MIT License
 * 
 * Copyright (c) 2025 Choco-Technologies
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 * @brief DMOD VFS - Virtual File System
 * @date 2025-10-15
 * @author Choco-Technologies
 * 
 * @file dmod_vfs.h
 * 
 * @defgroup DMOD_VFS DMOD VFS
 * 
 * @version 1.0
 * 
 * The DMOD VFS is a virtual file system implementation that allows mounting
 * multiple file systems implementing the FSI (File System Interface) from dmod-fsi.
 * 
 * This library is designed to be integrated into dmod-boot and provides the
 * implementation of file operations required by DMOD (Dmod_FileOpen, etc.).
 * 
 * The VFS manages mounted file systems and routes file operations to the
 * appropriate file system based on mount points.
 */
#ifndef INC_DMOD_VFS_H_
#define INC_DMOD_VFS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdbool.h>

// Forward declarations
typedef struct DmodVfs_MountPoint_s DmodVfs_MountPoint_t;
typedef struct DmodVfs_FileHandle_s DmodVfs_FileHandle_t;

//==============================================================================
//                              CONFIGURATION
//==============================================================================

#ifndef DMOD_VFS_MAX_MOUNT_POINTS
#define DMOD_VFS_MAX_MOUNT_POINTS 8
#endif

#ifndef DMOD_VFS_MAX_PATH_LENGTH
#define DMOD_VFS_MAX_PATH_LENGTH 256
#endif

//==============================================================================
//                              API FUNCTIONS
//==============================================================================

/**
 * @addtogroup DMOD_VFS
 * @{
 */

/**
 * @brief Initialize the VFS system
 * @return true on success, false otherwise
 */
bool DmodVfs_Init(void);

/**
 * @brief Deinitialize the VFS system
 */
void DmodVfs_Deinit(void);

/**
 * @brief Mount a file system at a given mount point
 * @param mountPoint Mount point path (e.g., "/dev", "/ram")
 * @param fsName Name of the file system module to mount
 * @return true on success, false otherwise
 */
bool DmodVfs_Mount(const char* mountPoint, const char* fsName);

/**
 * @brief Unmount a file system
 * @param mountPoint Mount point path
 * @return true on success, false otherwise
 */
bool DmodVfs_Unmount(const char* mountPoint);

/**
 * @brief Check if a path is mounted
 * @param path Path to check
 * @return true if mounted, false otherwise
 */
bool DmodVfs_IsMounted(const char* path);

/**
 * @brief Open a file
 * @param path Path to the file
 * @param mode Open mode (e.g., "r", "w", "a", "rb", "wb")
 * @return File handle or NULL on error
 */
void* DmodVfs_FileOpen(const char* path, const char* mode);

/**
 * @brief Read from a file
 * @param buffer Buffer to read into
 * @param size Size of each element
 * @param count Number of elements to read
 * @param file File handle
 * @return Number of elements actually read
 */
size_t DmodVfs_FileRead(void* buffer, size_t size, size_t count, void* file);

/**
 * @brief Write to a file
 * @param buffer Buffer to write from
 * @param size Size of each element
 * @param count Number of elements to write
 * @param file File handle
 * @return Number of elements actually written
 */
size_t DmodVfs_FileWrite(const void* buffer, size_t size, size_t count, void* file);

/**
 * @brief Seek to a position in a file
 * @param file File handle
 * @param offset Offset to seek to
 * @param origin Seek origin (SEEK_SET, SEEK_CUR, SEEK_END)
 * @return 0 on success, non-zero on error
 */
int DmodVfs_FileSeek(void* file, long offset, int origin);

/**
 * @brief Get current file position
 * @param file File handle
 * @return Current position
 */
size_t DmodVfs_FileTell(void* file);

/**
 * @brief Get file size
 * @param file File handle
 * @return File size in bytes
 */
size_t DmodVfs_FileSize(void* file);

/**
 * @brief Close a file
 * @param file File handle
 */
void DmodVfs_FileClose(void* file);

/**
 * @brief Check if a file is available
 * @param path Path to the file
 * @return true if file exists, false otherwise
 */
bool DmodVfs_FileAvailable(const char* path);

//! @}

#ifdef __cplusplus
}
#endif

#endif /* INC_DMOD_VFS_H_ */
