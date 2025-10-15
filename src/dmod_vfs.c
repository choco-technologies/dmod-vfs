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
 */

#include "dmod_vfs.h"
#include <string.h>

// Forward declarations for DMOD types
typedef struct Dmod_Context_s Dmod_Context_t;

// Declare minimal DMOD functions we need
extern void* Dmod_Malloc(size_t Size);
extern void Dmod_Free(void* Ptr);
extern int Dmod_Printf(const char* Format, ...);
extern Dmod_Context_t* Dmod_GetNextDifModule(const char* DifSignature, Dmod_Context_t* Previous);
extern void* Dmod_GetDifFunction(Dmod_Context_t* Context, const char* DifSignature);

// FSI DIF signature declarations (from dmod-fsi)
// These would normally be provided by including fsi.h
extern const char* const dmod_fsi_fopen_sig;
extern const char* const dmod_fsi_fclose_sig;
extern const char* const dmod_fsi_fread_sig;
extern const char* const dmod_fsi_fwrite_sig;
extern const char* const dmod_fsi_lseek_sig;
extern const char* const dmod_fsi_tell_sig;
extern const char* const dmod_fsi_size_sig;
extern const char* const dmod_fsi_stat_sig;

// Define logging macros
#ifndef NDEBUG
#   define DMOD_LOG_INFO(...)    Dmod_Printf("[INFO] " __VA_ARGS__)
#   define DMOD_LOG_ERROR(...)   Dmod_Printf("[ERROR] " __VA_ARGS__)
#else
#   define DMOD_LOG_INFO(...)    ((void)0)
#   define DMOD_LOG_ERROR(...)   ((void)0)
#endif

//==============================================================================
//                              TYPE DEFINITIONS
//==============================================================================

/**
 * @brief File system function pointers
 */
typedef struct {
    void* (*fopen)(void** fp, const char* path, int mode, int attr);
    int (*fclose)(void* fp);
    int (*fread)(void* fp, void* buffer, size_t size, size_t* read);
    int (*fwrite)(void* fp, const void* buffer, size_t size, size_t* written);
    long (*lseek)(void* fp, long offset, int whence);
    long (*tell)(void* fp);
    long (*size)(void* fp);
    int (*stat)(const char* path, void* stat);
} DmodVfs_FsOps_t;

/**
 * @brief Mount point structure
 */
struct DmodVfs_MountPoint_s {
    char mountPoint[DMOD_VFS_MAX_PATH_LENGTH];
    char fsName[64];
    Dmod_Context_t* fsContext;
    DmodVfs_FsOps_t ops;
    bool active;
};

/**
 * @brief File handle structure
 */
struct DmodVfs_FileHandle_s {
    void* fsHandle;
    DmodVfs_MountPoint_t* mountPoint;
};

//==============================================================================
//                              STATIC VARIABLES
//==============================================================================

static DmodVfs_MountPoint_t s_mountPoints[DMOD_VFS_MAX_MOUNT_POINTS];
static bool s_initialized = false;

//==============================================================================
//                              HELPER FUNCTIONS
//==============================================================================

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

        size_t mpLen = strlen(s_mountPoints[i].mountPoint);
        
        // Check if path starts with this mount point
        if (strncmp(path, s_mountPoints[i].mountPoint, mpLen) == 0) {
            // Ensure it's a proper mount point match (next char is '/' or '\0')
            if (path[mpLen] == '/' || path[mpLen] == '\0') {
                if (mpLen > bestMatchLen) {
                    bestMatchLen = mpLen;
                    bestMatch = &s_mountPoints[i];
                }
            }
        }
    }

    return bestMatch;
}

/**
 * @brief Get relative path within mount point
 */
static const char* GetRelativePath(const char* fullPath, const char* mountPoint)
{
    size_t mpLen = strlen(mountPoint);
    const char* relativePath = fullPath + mpLen;
    
    // Skip leading slash
    while (*relativePath == '/') {
        relativePath++;
    }
    
    // If empty, return "/"
    if (*relativePath == '\0') {
        return "/";
    }
    
    return relativePath;
}

/**
 * @brief Convert fopen mode to FSI mode flags
 */
static int ModeToFlags(const char* mode)
{
    int flags = 0;
    
    if (!mode) {
        return 0;
    }
    
    // FSI_O_RDONLY = 0x0001, FSI_O_WRONLY = 0x0002, FSI_O_RDWR = 0x0003
    // FSI_O_CREAT = 0x0100, FSI_O_TRUNC = 0x0200, FSI_O_APPEND = 0x0400
    
    if (mode[0] == 'r') {
        if (strchr(mode, '+')) {
            flags = 0x0003;  // FSI_O_RDWR
        } else {
            flags = 0x0001;  // FSI_O_RDONLY
        }
    } else if (mode[0] == 'w') {
        if (strchr(mode, '+')) {
            flags = 0x0003 | 0x0100 | 0x0200;  // FSI_O_RDWR | FSI_O_CREAT | FSI_O_TRUNC
        } else {
            flags = 0x0002 | 0x0100 | 0x0200;  // FSI_O_WRONLY | FSI_O_CREAT | FSI_O_TRUNC
        }
    } else if (mode[0] == 'a') {
        if (strchr(mode, '+')) {
            flags = 0x0003 | 0x0100 | 0x0400;  // FSI_O_RDWR | FSI_O_CREAT | FSI_O_APPEND
        } else {
            flags = 0x0002 | 0x0100 | 0x0400;  // FSI_O_WRONLY | FSI_O_CREAT | FSI_O_APPEND
        }
    }
    
    return flags;
}

//==============================================================================
//                              PUBLIC FUNCTIONS
//==============================================================================

bool DmodVfs_Init(void)
{
    if (s_initialized) {
        return true;
    }

    // Initialize mount points
    for (int i = 0; i < DMOD_VFS_MAX_MOUNT_POINTS; i++) {
        s_mountPoints[i].active = false;
        s_mountPoints[i].mountPoint[0] = '\0';
        s_mountPoints[i].fsName[0] = '\0';
        s_mountPoints[i].fsContext = NULL;
    }

    s_initialized = true;
    DMOD_LOG_INFO("VFS initialized\n");
    return true;
}

void DmodVfs_Deinit(void)
{
    if (!s_initialized) {
        return;
    }

    // Unmount all file systems
    for (int i = 0; i < DMOD_VFS_MAX_MOUNT_POINTS; i++) {
        if (s_mountPoints[i].active) {
            DmodVfs_Unmount(s_mountPoints[i].mountPoint);
        }
    }

    s_initialized = false;
    DMOD_LOG_INFO("VFS deinitialized\n");
}

bool DmodVfs_Mount(const char* mountPoint, const char* fsName)
{
    if (!s_initialized) {
        DMOD_LOG_ERROR("VFS not initialized\n");
        return false;
    }

    if (!mountPoint || !fsName) {
        DMOD_LOG_ERROR("Invalid mount parameters\n");
        return false;
    }

    // Check if already mounted
    for (int i = 0; i < DMOD_VFS_MAX_MOUNT_POINTS; i++) {
        if (s_mountPoints[i].active && 
            strcmp(s_mountPoints[i].mountPoint, mountPoint) == 0) {
            DMOD_LOG_ERROR("Mount point already in use: %s\n", mountPoint);
            return false;
        }
    }

    // Find free mount point slot
    int freeSlot = -1;
    for (int i = 0; i < DMOD_VFS_MAX_MOUNT_POINTS; i++) {
        if (!s_mountPoints[i].active) {
            freeSlot = i;
            break;
        }
    }

    if (freeSlot < 0) {
        DMOD_LOG_ERROR("No free mount point slots\n");
        return false;
    }

    // Find the file system module using DIF interface
    // Search through all modules implementing the FSI interface
    Dmod_Context_t* fsContext = NULL;
    
    // NOTE: In a full implementation, we would search for a specific module by name
    // For now, we use the first FSI module found (or could match by name if needed)
    // The actual module discovery would be done via Dmod_GetNextDifModule with FSI signatures
    DMOD_LOG_INFO("Attempting to mount %s at %s\n", fsName, mountPoint);

    // Try to find an FSI module
    // We use a weak external symbol that will be NULL if not linked
    if (dmod_fsi_fopen_sig) {
        fsContext = Dmod_GetNextDifModule(dmod_fsi_fopen_sig, NULL);
        
        if (fsContext) {
            DMOD_LOG_INFO("Found FSI module for mounting\n");
        } else {
            DMOD_LOG_ERROR("No FSI module found for mounting\n");
        }
    } else {
        DMOD_LOG_ERROR("FSI interface not available (fsi.h not linked)\n");
    }

    DmodVfs_MountPoint_t* mp = &s_mountPoints[freeSlot];
    strncpy(mp->mountPoint, mountPoint, DMOD_VFS_MAX_PATH_LENGTH - 1);
    mp->mountPoint[DMOD_VFS_MAX_PATH_LENGTH - 1] = '\0';
    strncpy(mp->fsName, fsName, sizeof(mp->fsName) - 1);
    mp->fsName[sizeof(mp->fsName) - 1] = '\0';
    mp->fsContext = fsContext;
    mp->active = true;

    // Get function pointers from DIF if module is available
    if (fsContext && dmod_fsi_fopen_sig) {
        mp->ops.fopen = (void* (*)(void**, const char*, int, int))
            Dmod_GetDifFunction(fsContext, dmod_fsi_fopen_sig);
        mp->ops.fclose = (int (*)(void*))
            Dmod_GetDifFunction(fsContext, dmod_fsi_fclose_sig);
        mp->ops.fread = (int (*)(void*, void*, size_t, size_t*))
            Dmod_GetDifFunction(fsContext, dmod_fsi_fread_sig);
        mp->ops.fwrite = (int (*)(void*, const void*, size_t, size_t*))
            Dmod_GetDifFunction(fsContext, dmod_fsi_fwrite_sig);
        mp->ops.lseek = (long (*)(void*, long, int))
            Dmod_GetDifFunction(fsContext, dmod_fsi_lseek_sig);
        mp->ops.tell = (long (*)(void*))
            Dmod_GetDifFunction(fsContext, dmod_fsi_tell_sig);
        mp->ops.size = (long (*)(void*))
            Dmod_GetDifFunction(fsContext, dmod_fsi_size_sig);
        mp->ops.stat = (int (*)(const char*, void*))
            Dmod_GetDifFunction(fsContext, dmod_fsi_stat_sig);
            
        DMOD_LOG_INFO("Initialized function pointers from FSI module\n");
    } else {
        // No module available, operations will fail
        mp->ops.fopen = NULL;
        mp->ops.fclose = NULL;
        mp->ops.fread = NULL;
        mp->ops.fwrite = NULL;
        mp->ops.lseek = NULL;
        mp->ops.tell = NULL;
        mp->ops.size = NULL;
        mp->ops.stat = NULL;
        
        DMOD_LOG_ERROR("No FSI module available - operations will not work\n");
    }

    DMOD_LOG_INFO("Mounted %s at %s\n", fsName, mountPoint);
    return true;
}

bool DmodVfs_Unmount(const char* mountPoint)
{
    if (!s_initialized) {
        DMOD_LOG_ERROR("VFS not initialized\n");
        return false;
    }

    if (!mountPoint) {
        return false;
    }

    for (int i = 0; i < DMOD_VFS_MAX_MOUNT_POINTS; i++) {
        if (s_mountPoints[i].active && 
            strcmp(s_mountPoints[i].mountPoint, mountPoint) == 0) {
            
            DMOD_LOG_INFO("Unmounting %s\n", mountPoint);
            s_mountPoints[i].active = false;
            s_mountPoints[i].fsContext = NULL;
            return true;
        }
    }

    DMOD_LOG_ERROR("Mount point not found: %s\n", mountPoint);
    return false;
}

bool DmodVfs_IsMounted(const char* path)
{
    if (!s_initialized) {
        return false;
    }

    return FindMountPoint(path) != NULL;
}

void* DmodVfs_FileOpen(const char* path, const char* mode)
{
    if (!s_initialized) {
        DMOD_LOG_ERROR("VFS not initialized\n");
        return NULL;
    }

    if (!path || !mode) {
        return NULL;
    }

    DmodVfs_MountPoint_t* mp = FindMountPoint(path);
    if (!mp) {
        DMOD_LOG_ERROR("No mount point found for path: %s\n", path);
        return NULL;
    }

    if (!mp->ops.fopen) {
        DMOD_LOG_ERROR("File system does not support fopen\n");
        return NULL;
    }

    const char* relativePath = GetRelativePath(path, mp->mountPoint);
    int flags = ModeToFlags(mode);
    
    void* fsHandle = NULL;
    mp->ops.fopen(&fsHandle, relativePath, flags, 0);
    
    if (!fsHandle) {
        return NULL;
    }

    // Allocate VFS file handle
    DmodVfs_FileHandle_t* handle = (DmodVfs_FileHandle_t*)Dmod_Malloc(sizeof(DmodVfs_FileHandle_t));
    if (!handle) {
        mp->ops.fclose(fsHandle);
        return NULL;
    }

    handle->fsHandle = fsHandle;
    handle->mountPoint = mp;

    return handle;
}

size_t DmodVfs_FileRead(void* buffer, size_t size, size_t count, void* file)
{
    if (!file || !buffer) {
        return 0;
    }

    DmodVfs_FileHandle_t* handle = (DmodVfs_FileHandle_t*)file;
    DmodVfs_MountPoint_t* mp = handle->mountPoint;

    if (!mp->ops.fread) {
        return 0;
    }

    size_t totalSize = size * count;
    size_t bytesRead = 0;
    
    int result = mp->ops.fread(handle->fsHandle, buffer, totalSize, &bytesRead);
    if (result != 0) {
        return 0;
    }

    return bytesRead / size;
}

size_t DmodVfs_FileWrite(const void* buffer, size_t size, size_t count, void* file)
{
    if (!file || !buffer) {
        return 0;
    }

    DmodVfs_FileHandle_t* handle = (DmodVfs_FileHandle_t*)file;
    DmodVfs_MountPoint_t* mp = handle->mountPoint;

    if (!mp->ops.fwrite) {
        return 0;
    }

    size_t totalSize = size * count;
    size_t bytesWritten = 0;
    
    int result = mp->ops.fwrite(handle->fsHandle, buffer, totalSize, &bytesWritten);
    if (result != 0) {
        return 0;
    }

    return bytesWritten / size;
}

int DmodVfs_FileSeek(void* file, long offset, int origin)
{
    if (!file) {
        return -1;
    }

    DmodVfs_FileHandle_t* handle = (DmodVfs_FileHandle_t*)file;
    DmodVfs_MountPoint_t* mp = handle->mountPoint;

    if (!mp->ops.lseek) {
        return -1;
    }

    long result = mp->ops.lseek(handle->fsHandle, offset, origin);
    return (result < 0) ? -1 : 0;
}

size_t DmodVfs_FileTell(void* file)
{
    if (!file) {
        return 0;
    }

    DmodVfs_FileHandle_t* handle = (DmodVfs_FileHandle_t*)file;
    DmodVfs_MountPoint_t* mp = handle->mountPoint;

    if (!mp->ops.tell) {
        return 0;
    }

    long result = mp->ops.tell(handle->fsHandle);
    return (result < 0) ? 0 : (size_t)result;
}

size_t DmodVfs_FileSize(void* file)
{
    if (!file) {
        return 0;
    }

    DmodVfs_FileHandle_t* handle = (DmodVfs_FileHandle_t*)file;
    DmodVfs_MountPoint_t* mp = handle->mountPoint;

    if (!mp->ops.size) {
        return 0;
    }

    long result = mp->ops.size(handle->fsHandle);
    return (result < 0) ? 0 : (size_t)result;
}

void DmodVfs_FileClose(void* file)
{
    if (!file) {
        return;
    }

    DmodVfs_FileHandle_t* handle = (DmodVfs_FileHandle_t*)file;
    DmodVfs_MountPoint_t* mp = handle->mountPoint;

    if (mp->ops.fclose) {
        mp->ops.fclose(handle->fsHandle);
    }

    Dmod_Free(handle);
}

bool DmodVfs_FileAvailable(const char* path)
{
    if (!s_initialized || !path) {
        return false;
    }

    DmodVfs_MountPoint_t* mp = FindMountPoint(path);
    if (!mp) {
        return false;
    }

    if (!mp->ops.stat) {
        return false;
    }

    const char* relativePath = GetRelativePath(path, mp->mountPoint);
    
    // Try to stat the file
    int result = mp->ops.stat(relativePath, NULL);
    return (result == 0);
}
