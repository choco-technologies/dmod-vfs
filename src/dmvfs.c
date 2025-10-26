#include "dmvfs.h"
#include <string.h>

typedef struct {
    Dmod_Context_t* fs_context;
    char* mount_point;
    dmfsi_context_t mount_context;
} mount_point_t;

typedef struct {
    mount_point_t* mount_point;
    void* fs_file;
} file_t;

static mount_point_t* g_mount_points = NULL;
static int g_max_mount_points = 0;
static int g_max_open_files = 0;
static void* g_mutex = NULL;
static const char* g_cwd = NULL;
static const char* g_pwd = NULL;
static file_t* g_open_files = NULL;

/**
 * @brief Check if DMVFS is initialized
 * @return true if initialized, false otherwise
 */
static inline bool is_initialized(void)
{
    return (g_mount_points != NULL);
}

/**
 * @brief Duplicate a string
 * @param str String to duplicate
 * @return Pointer to duplicated string, or NULL on failure
 */
static const char* duplicate_string(const char* str)
{
    if(str == NULL)
    {
        return NULL;
    }

    char* dup = (char*)Dmod_Malloc(strlen(str) + 1);
    if(dup != NULL)
    {
        strcpy(dup, str);
    }
    return dup;
}

/**
 * @brief Update a string (free old and duplicate new)
 * @param old_str Old string to free
 * @param new_str New string to duplicate
 * @return Pointer to duplicated new string, or NULL on failure
 */
static const char* update_string(const char* old_str, const char* new_str)
{
    if(old_str != NULL)
    {
        Dmod_Free((void*)old_str);
    }
    return duplicate_string(new_str);
}

/**
 * @brief Convert path to absolute path
 * @param path Input path
 * @return Pointer to absolute path, or NULL on failure
 */
static const char* to_absolute_path(const char* path)
{
    if(path == NULL)
    {
        return NULL;
    }

    if(path[0] == '/')
    {
        return duplicate_string(path);
    }
    else
    {
        size_t cwd_len = (g_cwd != NULL) ? strlen(g_cwd) : 0;
        size_t path_len = strlen(path);
        char* abs_path = (char*)Dmod_Malloc(cwd_len + 1 + path_len + 1);
        if(abs_path != NULL)
        {
            if(cwd_len > 0)
            {
                strcpy(abs_path, g_cwd);
                abs_path[cwd_len] = '/';
                strcpy(abs_path + cwd_len + 1, path);
            }
            else
            {
                strcpy(abs_path, "/");
                strcpy(abs_path + 1, path);
            }
        }
        return abs_path;
    }
}

/**
 * @brief Find free mount point entry
 * @return Pointer to free mount point entry, or NULL if none available
 */
static mount_point_t* find_free_mount_point(void)
{
    if(!is_initialized())
    {
        DMOD_LOG_ERROR("DMVFS is not initialized");
        return NULL;
    }

    for(int i = 0; i < g_max_mount_points; i++)
    {
        if(g_mount_points[i].mount_point == NULL)
        {
            return &g_mount_points[i];
        }
    }

    DMOD_LOG_ERROR("No free mount points available");
    return NULL;
}

/**
 * @brief Find mount point by path
 * @param mount_point Mount point path
 * @return Pointer to the mount point entry, or NULL if not found
 */
static mount_point_t* find_mount_point(const char* mount_point)
{
    if(!is_initialized())
    {
        DMOD_LOG_ERROR("DMVFS is not initialized");
        return NULL;
    }

    for(int i = 0; i < g_max_mount_points; i++)
    {
        if(g_mount_points[i].mount_point != NULL &&
           strcmp(g_mount_points[i].mount_point, mount_point) == 0)
        {
            return &g_mount_points[i];
        }
    }

    DMOD_LOG_WARN("Mount point '%s' not found", mount_point);
    return NULL;
}

/**
 * @brief Get mount point for a given path
 * @param path File path
 * @return Pointer to the mount point entry, or NULL if not found
 */
static mount_point_t* get_mount_point_for_path(const char* path)
{
    if(!is_initialized())
    {
        DMOD_LOG_ERROR("DMVFS is not initialized");
        return NULL;
    }

    for(int i = 0; i < g_max_mount_points; i++)
    {
        if(g_mount_points[i].mount_point != NULL &&
           strncmp(path, g_mount_points[i].mount_point, strlen(g_mount_points[i].mount_point)) == 0)
        {
            return &g_mount_points[i];
        }
    }

    DMOD_LOG_WARN("No mount point found for path '%s'", path);
    return NULL;
}

/**
 * @brief Find free file entry
 * @return Pointer to free file entry, or NULL if none available
 */
static file_t* find_free_file_entry(void)
{
    if(!is_initialized())
    {
        DMOD_LOG_ERROR("DMVFS is not initialized");
        return NULL;
    }

    for(int i = 0; i < g_max_open_files; i++)
    {
        if(g_open_files[i].mount_point == NULL)
        {
            return &g_open_files[i];
        }
    }

    DMOD_LOG_ERROR("No free file entries available");
    return NULL;
}

/**
 * @brief Close all files of a given mount point
 * @param mp_entry Pointer to the mount point entry
 * @return true on success, false on failure
 */
static bool close_all_file_of_mount_point(mount_point_t* mp_entry)
{
    if(!is_initialized())
    {
        DMOD_LOG_ERROR("DMVFS is not initialized");
        return false;
    }

    for(int i = 0; i < g_max_open_files; i++)
    {
        if(g_open_files[i].mount_point == mp_entry)
        {
            dmod_dmfsi_fclose_t close_func = (dmod_dmfsi_fclose_t)Dmod_GetDifFunction(mp_entry->fs_context, dmod_dmfsi_fclose_sig);
            if(close_func != NULL)
            {
                if(!close_func(mp_entry->mount_context, g_open_files[i].fs_file))
                {
                    DMOD_LOG_ERROR("Failed to close file in mount point '%s'", mp_entry->mount_point);
                    return false;
                }
            }

            g_open_files[i].mount_point = NULL;
            g_open_files[i].fs_file = NULL;
        }
    }

    return true;
}

/**
 * @brief Find file system by name
 * @param fs_name Name of the file system
 * @return Pointer to the file system context, or NULL if not found
 */
static Dmod_Context_t* find_fs_by_name(const char* fs_name)
{
    if(!is_initialized())
    {
        DMOD_LOG_ERROR("DMVFS is not initialized");
        return NULL;
    }

    if( Dmod_Mutex_Lock(g_mutex) != 0 )
    {
        DMOD_LOG_ERROR("Failed to lock DMVFS mutex");
        return NULL;
    }

    Dmod_Context_t* fs_context = Dmod_GetNextDifModule(dmod_dmfsi_fopen_sig, NULL);
    while(fs_context != NULL)
    {
        if(fs_context->Header != NULL && strcmp(fs_context->Header->Name, fs_name) == 0)
        {
            DMOD_LOG_VERBOSE("File system '%s' found", fs_name);
            Dmod_Mutex_Unlock(g_mutex);
            return fs_context;
        }
        fs_context = Dmod_GetNextDifModule(dmod_dmfsi_fopen_sig, fs_context);
    }

    DMOD_LOG_WARN("File system '%s' not found", fs_name);
    Dmod_Mutex_Unlock(g_mutex);
    return NULL;
}

/**
 * @brief Add mount point
 * @param mount_point Mount point path
 * @param fs_context File system context
 * @param config Configuration string for the file system (file system specific)
 * @return Pointer to the mount point entry, or NULL on failure
 */
static mount_point_t* add_mount_point(const char* mount_point, Dmod_Context_t* fs_context, const char* config)
{
    if(!is_initialized())
    {
        DMOD_LOG_ERROR("DMVFS is not initialized");
        return NULL;
    }

    if(fs_context == NULL)
    {
        DMOD_LOG_ERROR("Cannot add mount point '%s': Invalid file system context", mount_point);
        return NULL;
    }

    const char* module_name = Dmod_GetName(fs_context);
    if(module_name == NULL)
    {
        DMOD_LOG_ERROR("Cannot add mount point '%s': Failed to get module name", mount_point);
        return NULL;
    }

    Dmod_BeginUsage(module_name);

    mount_point_t* free_entry = find_free_mount_point();
    if(free_entry == NULL)
    {
        return NULL;
    }

    dmod_dmfsi_init_t init_func = (dmod_dmfsi_init_t)Dmod_GetDifFunction(fs_context, dmod_dmfsi_init_sig);
    if(init_func != NULL)
    {
        free_entry->mount_context = init_func(config);
        if(free_entry->mount_context == NULL)
        {
            DMOD_LOG_ERROR("Failed to initialize mount context for mount point '%s'", mount_point);
            return NULL;
        }
    }

    free_entry->mount_point = Dmod_Malloc(strlen(mount_point) + 1);
    if(free_entry->mount_point == NULL)
    {
        DMOD_LOG_ERROR("Failed to allocate memory for mount point");
        return NULL;
    }


    strcpy(free_entry->mount_point, mount_point);
    free_entry->fs_context = fs_context;
    return free_entry;
    return NULL;
}

/**
 * @brief Remove mount point
 * @param mount_point Mount point path
 * @return true on success, false on failure
 */
static bool remove_mount_point(const char* mount_point)
{
    if(!is_initialized())
    {
        DMOD_LOG_ERROR("DMVFS is not initialized");
        return false;
    }

    mount_point_t* mp_entry = find_mount_point(mount_point);
    if(mp_entry == NULL)
    {
        DMOD_LOG_ERROR("Mount point '%s' not found", mount_point);
        return false;
    }

    dmod_dmfsi_deinit_t deinit_func = (dmod_dmfsi_deinit_t)Dmod_GetDifFunction(mp_entry->fs_context, dmod_dmfsi_deinit_sig);
    if(deinit_func != NULL)
    {
        if(!deinit_func(mp_entry->mount_context))
        {
            DMOD_LOG_ERROR("Failed to deinitialize mount context for mount point '%s'", mount_point);
        }
    }

    const char* module_name = Dmod_GetName(mp_entry->fs_context);
    if(module_name == NULL)
    {
        DMOD_LOG_ERROR("Cannot remove mount point '%s': Failed to get module name", mount_point);
        return false;
    }

    Dmod_EndUsage(module_name);

    Dmod_Free(mp_entry->mount_point);
    mp_entry->mount_point = NULL;
    mp_entry->mount_context = NULL;
    mp_entry->fs_context = NULL;
    return true;
}

/**
 * @brief Initialize DMVFS
 * 
 * The function initializes the DMVFS with a specified maximum number of mount points.
 * It allocates memory for the mount points and creates a mutex for thread safety.
 * @param max_mount_points Maximum number of mount points
 * 
 * @return true on success, false on failure
 */
DMOD_INPUT_API_DECLARATION(dmvfs, 1.0, bool, _init, (int max_mount_points, int max_open_files))
{
    if (is_initialized())
    {
        DMOD_LOG_WARN("DMVFS is already initialized");
        return false;
    }

    if (max_mount_points <= 0)
    {
        DMOD_LOG_ERROR("Invalid maximum mount points: %d", max_mount_points);
        return false;
    }

    g_mount_points = (mount_point_t*)Dmod_Malloc(sizeof(mount_point_t) * max_mount_points);
    if (g_mount_points == NULL)
    {
        DMOD_LOG_ERROR("Failed to allocate memory for mount points");
        return false;
    }

    g_open_files = (file_t*)Dmod_Malloc(sizeof(file_t) * max_open_files);
    if (g_open_files == NULL)
    {
        DMOD_LOG_ERROR("Failed to allocate memory for open files");
        Dmod_Free(g_mount_points);
        g_mount_points = NULL;
        return false;
    }

    memset(g_mount_points, 0, sizeof(mount_point_t) * max_mount_points);
    g_max_mount_points = max_mount_points;
    g_max_open_files = max_open_files;

    g_mutex = Dmod_Mutex_New(true);
    if (g_mutex == NULL)
    {
        DMOD_LOG_ERROR("Failed to create mutex");
        Dmod_Free(g_mount_points);
        g_mount_points = NULL;
        g_max_mount_points = 0;
        return false;
    }

    g_cwd = duplicate_string("/");
    g_pwd = duplicate_string("/");
    if (g_cwd == NULL || g_pwd == NULL)
    {
        DMOD_LOG_ERROR("Failed to allocate memory for CWD or PWD");
        Dmod_Free(g_mount_points);
        g_mount_points = NULL;
        g_max_mount_points = 0;
        if (g_cwd) Dmod_Free((void*)g_cwd);
        if (g_pwd) Dmod_Free((void*)g_pwd);
        g_cwd = NULL;
        g_pwd = NULL;
        Dmod_Mutex_Delete(g_mutex);
        g_mutex = NULL;
        return false;
    }

    DMOD_LOG_INFO("DMVFS initialized with max mount points: %d", max_mount_points);
    return true;
}

/**
 * @brief Deinitialize DMVFS
 * 
 * The function deinitializes the DMVFS by freeing allocated resources,
 * including the mount points and the mutex. It ensures that the DMVFS
 * is properly cleaned up and ready for reinitialization if needed.
 * 
 * @return true on success, false on failure
 */
DMOD_INPUT_API_DECLARATION(dmvfs, 1.0, bool, _deinit, (void))
{
    if (!is_initialized())
    {
        DMOD_LOG_WARN("DMVFS is not initialized");
        return false;
    }

    if (Dmod_Mutex_Lock(g_mutex) != 0)
    {
        DMOD_LOG_ERROR("Failed to lock DMVFS mutex");
        return false;
    }

    // Free all mount points
    for (int i = 0; i < g_max_mount_points; i++)
    {
        if (g_mount_points[i].mount_point != NULL)
        {
            remove_mount_point(g_mount_points[i].mount_point);
        }
    }

    // Free the mount points array
    Dmod_Free(g_mount_points);
    Dmod_Free(g_open_files);
    Dmod_Free(g_cwd);
    Dmod_Free(g_pwd);
    g_mount_points = NULL;
    g_max_mount_points = 0;

    // Destroy the mutex
    Dmod_Mutex_Unlock(g_mutex);
    Dmod_Mutex_Delete(g_mutex);
    g_mutex = NULL;

    DMOD_LOG_INFO("DMVFS deinitialized successfully");
    return true;
}

/**
 * @brief Get maximum number of mount points
 * 
 * @return Maximum number of mount points
 */
DMOD_INPUT_API_DECLARATION(dmvfs, 1.0, int, _get_max_mount_points, (void))
{
    return g_max_mount_points;
}

/**
 * @brief Mount file system
 * 
 * The function mounts a file system by its name at the specified mount point.
 * It initializes the file system using its init function and registers the
 * mount point in the DMVFS.
 * 
 * @param fs_name Name of the file system to mount
 * @param mount_point Mount point path
 * @param config Configuration string for the file system (file system specific)
 * @return true on success, false on failure
 */
DMOD_INPUT_API_DECLARATION(dmvfs, 1.0, bool, _mount_fs, (const char* fs_name, const char* mount_point, const char* config))
{   
    if(!is_initialized())
    {
        DMOD_LOG_ERROR("DMVFS is not initialized");
        return false;
    }

    if(Dmod_Mutex_Lock(g_mutex) != 0)
    {
        DMOD_LOG_ERROR("Failed to lock DMVFS mutex");
        return false;
    }

    Dmod_Context_t* fs_context = find_fs_by_name(fs_name);
    if(fs_context == NULL)
    {
        DMOD_LOG_ERROR("Cannot mount file system '%s': Not found", fs_name);
        Dmod_Mutex_Unlock(g_mutex);
        return false;
    }

    mount_point_t* mp_entry = add_mount_point(mount_point, fs_context, config);
    if(mp_entry == NULL)
    {
        DMOD_LOG_ERROR("Cannot mount file system '%s'", fs_name);
        Dmod_Mutex_Unlock(g_mutex);
        return false;
    }

    Dmod_Mutex_Unlock(g_mutex);
    DMOD_LOG_INFO("File system '%s' mounted at '%s' successfully", fs_name, mount_point);
    return true;
}

/**
 * @brief Unmount file system
 * 
 * The function unmounts a file system at the specified mount point.
 * It deinitializes the file system using its deinit function and removes
 * the mount point from the DMVFS.
 * 
 * @param mount_point Mount point path
 * @return true on success, false on failure
 */
DMOD_INPUT_API_DECLARATION(dmvfs, 1.0, bool, _unmount_fs, (const char* mount_point))
{
    if(!is_initialized())
    {
        DMOD_LOG_ERROR("DMVFS is not initialized");
        return false;
    }

    if(Dmod_Mutex_Lock(g_mutex) != 0)
    {
        DMOD_LOG_ERROR("Failed to lock DMVFS mutex");
        return false;
    }

    if(!remove_mount_point(mount_point))
    {
        DMOD_LOG_ERROR("Cannot unmount file system at mount point '%s'", mount_point);
        Dmod_Mutex_Unlock(g_mutex);
        return false;
    }

    Dmod_Mutex_Unlock(g_mutex);
    DMOD_LOG_INFO("File system at mount point '%s' unmounted successfully", mount_point);
    return true;
}

// File operations
DMOD_INPUT_API_DECLARATION(dmvfs, 1.0, int, _fopen, (void** fp, const char* path, int mode, int attr, int pid))
{
    // TODO: Implement file open
    if (fp) *fp = NULL;
    return -1;
}

DMOD_INPUT_API_DECLARATION(dmvfs, 1.0, int, _fclose, (void* fp))
{
    // TODO: Implement file close
    return -1;
}

DMOD_INPUT_API_DECLARATION(dmvfs, 1.0, int, _fclose_process, (int pid))
{
    // TODO: Implement process file close
    return -1;
}

DMOD_INPUT_API_DECLARATION(dmvfs, 1.0, int, _fread, (void* fp, void* buf, size_t size, size_t* read_bytes))
{
    // TODO: Implement file read
    if (read_bytes) *read_bytes = 0;
    return -1;
}

DMOD_INPUT_API_DECLARATION(dmvfs, 1.0, int, _fwrite, (void* fp, const void* buf, size_t size, size_t* written_bytes))
{
    // TODO: Implement file write
    if (written_bytes) *written_bytes = 0;
    return -1;
}

DMOD_INPUT_API_DECLARATION(dmvfs, 1.0, int, _lseek, (void* fp, long offset, int whence))
{
    // TODO: Implement file seek
    return -1;
}

DMOD_INPUT_API_DECLARATION(dmvfs, 1.0, long, _ftell, (void* fp))
{
    // TODO: Implement file tell
    return -1;
}

DMOD_INPUT_API_DECLARATION(dmvfs, 1.0, int, _feof, (void* fp))
{
    // TODO: Implement file EOF check
    return 0;
}

DMOD_INPUT_API_DECLARATION(dmvfs, 1.0, int, _fflush, (void* fp))
{
    // TODO: Implement file flush
    return -1;
}

DMOD_INPUT_API_DECLARATION(dmvfs, 1.0, int, _error, (void* fp))
{
    // TODO: Implement error check
    return 0;
}

DMOD_INPUT_API_DECLARATION(dmvfs, 1.0, int, _remove, (const char* path))
{
    // TODO: Implement file removal
    return -1;
}

DMOD_INPUT_API_DECLARATION(dmvfs, 1.0, int, _rename, (const char* oldpath, const char* newpath))
{
    // TODO: Implement file rename
    return -1;
}

DMOD_INPUT_API_DECLARATION(dmvfs, 1.0, int, _ioctl, (void* fp, int command, void* arg))
{
    // TODO: Implement ioctl
    return -1;
}

DMOD_INPUT_API_DECLARATION(dmvfs, 1.0, int, _sync, (void* fp))
{
    // TODO: Implement file sync
    return -1;
}

DMOD_INPUT_API_DECLARATION(dmvfs, 1.0, int, _stat, (const char* path, dmod_fsi_stat_t* stat))
{
    // TODO: Implement file stat
    return -1;
}

DMOD_INPUT_API_DECLARATION(dmvfs, 1.0, int, _getc, (void* fp))
{
    // TODO: Implement character read
    return -1;
}

DMOD_INPUT_API_DECLARATION(dmvfs, 1.0, int, _putc, (void* fp, int c))
{
    // TODO: Implement character write
    return -1;
}

DMOD_INPUT_API_DECLARATION(dmvfs, 1.0, int, _chmod, (const char* path, int mode))
{
    // TODO: Implement chmod
    return -1;
}

DMOD_INPUT_API_DECLARATION(dmvfs, 1.0, int, _utime, (const char* path, uint32_t atime, uint32_t mtime))
{
    // TODO: Implement utime
    return -1;
}

DMOD_INPUT_API_DECLARATION(dmvfs, 1.0, int, _unlink, (const char* path))
{
    // TODO: Implement unlink
    return -1;
}

// Directory operations
DMOD_INPUT_API_DECLARATION(dmvfs, 1.0, int, _mkdir, (const char* path, int mode))
{
    // TODO: Implement mkdir
    return -1;
}

DMOD_INPUT_API_DECLARATION(dmvfs, 1.0, int, _rmdir, (const char* path))
{
    // TODO: Implement rmdir
    return -1;
}

DMOD_INPUT_API_DECLARATION(dmvfs, 1.0, int, _chdir, (const char* path))
{
    // TODO: Implement chdir
    return -1;
}

DMOD_INPUT_API_DECLARATION(dmvfs, 1.0, int, _opendir, (void** dp, const char* path))
{
    // TODO: Implement opendir
    if (dp) *dp = NULL;
    return -1;
}

DMOD_INPUT_API_DECLARATION(dmvfs, 1.0, int, _readdir, (void* dp, dmod_fsi_dir_entry_t* entry))
{
    // TODO: Implement readdir
    return -1;
}

DMOD_INPUT_API_DECLARATION(dmvfs, 1.0, int, _closedir, (void* dp))
{
    // TODO: Implement closedir
    return -1;
}

DMOD_INPUT_API_DECLARATION(dmvfs, 1.0, int, _direxists, (const char* path))
{
    // TODO: Implement directory existence check
    return 0;
}

// Current working directory
DMOD_INPUT_API_DECLARATION(dmvfs, 1.0, int, _getcwd, (char* buffer, size_t size))
{
    // TODO: Implement getcwd
    return -1;
}

DMOD_INPUT_API_DECLARATION(dmvfs, 1.0, int, _getpwd, (char* buffer, size_t size))
{
    // TODO: Implement getpwd
    return -1;
}

/**
 * @brief Convert a relative path to an absolute path
 * 
 * This function converts a given relative path to an absolute path
 * based on the current working directory (CWD). If the input path
 * is already absolute, it simply copies the path to the output buffer.
 * 
 * @param path Input path (relative or absolute)
 * @param abs_path Buffer to store the resulting absolute path
 * @param size Size of the abs_path buffer
 * 
 * @return 0 on success, -1 on failure (e.g., buffer too small or invalid input)
 */
DMOD_INPUT_API_DECLARATION(dmvfs, 1.0, int, _toabs, (const char* path, char* abs_path, size_t size))
{
    if (path == NULL || abs_path == NULL || size == 0)
    {
        DMOD_LOG_ERROR("Invalid arguments to _toabs");
        return -1;
    }

    if (path[0] == '/')
    {
        // Path is already absolute
        if (strlen(path) + 1 > size)
        {
            DMOD_LOG_ERROR("Buffer too small for absolute path");
            return -1;
        }
        strcpy(abs_path, path);
    }
    else
    {
        // Path is relative, prepend the current working directory
        if (!is_initialized() || g_cwd == NULL)
        {
            DMOD_LOG_ERROR("DMVFS is not initialized or CWD is NULL");
            return -1;
        }

        size_t cwd_len = strlen(g_cwd);
        size_t path_len = strlen(path);

        if (cwd_len + 1 + path_len + 1 > size)
        {
            DMOD_LOG_ERROR("Buffer too small for absolute path");
            return -1;
        }

        strncpy(abs_path, g_cwd, size - 1);
        abs_path[size - 1] = '\0';
        strncat(abs_path, "/", size - strlen(abs_path) - 1);
        strncat(abs_path, path, size - strlen(abs_path) - 1);
    }

    return 0;
}