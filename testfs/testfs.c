#define DMOD_ENABLE_REGISTRATION    ON
#ifndef DMOD_testfs
#   define DMOD_testfs
#endif

#include "dmod.h"
#include "dmfsi.h"
#include <string.h>

/**
 * @brief TestFS - Complete In-Memory File System for Testing
 * 
 * This is a complete implementation of the DMFSI interface for testing purposes.
 * All files and directories are stored in RAM with full API support.
 */

#define TESTFS_MAX_FILENAME  256
#define TESTFS_MAX_FILES     128
#define TESTFS_CONTEXT_MAGIC 0x54455354  // "TEST" in hex
#define TESTFS_MAX_PATH      512

// Context structure definition
struct dmfsi_context {
    uint32_t magic;
    void* root_dir;
    int initialized;
};

// File/Directory node types
typedef enum {
    NODE_TYPE_FILE = 0,
    NODE_TYPE_DIR = 1
} node_type_t;

// File/Directory node
typedef struct testfs_node_s {
    char name[TESTFS_MAX_FILENAME];
    node_type_t type;
    uint8_t* data;              // File data (NULL for directories)
    size_t size;                // Current size
    size_t capacity;            // Allocated capacity
    size_t position;            // Current read/write position
    int flags;                  // Open flags
    uint32_t attr;              // Attributes
    uint32_t ctime;             // Creation time
    uint32_t mtime;             // Modification time
    uint32_t atime;             // Access time
    struct testfs_node_s* parent;
    struct testfs_node_s* next_sibling;
    struct testfs_node_s* first_child;  // For directories
    int ref_count;              // Reference count for open files/dirs
} testfs_node_t;

// Directory iterator
typedef struct {
    testfs_node_t* dir;
    testfs_node_t* current;
} testfs_dir_iter_t;

// Helper: String operations
static int testfs_strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

static char* testfs_strncpy(char* dest, const char* src, size_t n) {
    size_t i;
    for (i = 0; i < n && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }
    for (; i < n; i++) {
        dest[i] = '\0';
    }
    return dest;
}

static void* testfs_memcpy(void* dest, const void* src, size_t n) {
    unsigned char* d = (unsigned char*)dest;
    const unsigned char* s = (const unsigned char*)src;
    for (size_t i = 0; i < n; i++) {
        d[i] = s[i];
    }
    return dest;
}

static size_t testfs_strlen(const char* s) {
    size_t len = 0;
    while (s[len] != '\0') {
        len++;
    }
    return len;
}

// Helper: Create a new node
static testfs_node_t* testfs_create_node(const char* name, node_type_t type) {
    testfs_node_t* node = (testfs_node_t*)Dmod_Malloc(sizeof(testfs_node_t));
    if (node == NULL) {
        return NULL;
    }
    
    testfs_strncpy(node->name, name, TESTFS_MAX_FILENAME - 1);
    node->name[TESTFS_MAX_FILENAME - 1] = '\0';
    node->type = type;
    node->data = NULL;
    node->size = 0;
    node->capacity = 0;
    node->position = 0;
    node->flags = 0;
    node->attr = (type == NODE_TYPE_DIR) ? DMFSI_ATTR_DIRECTORY : 0;
    node->ctime = 0;
    node->mtime = 0;
    node->atime = 0;
    node->parent = NULL;
    node->next_sibling = NULL;
    node->first_child = NULL;
    node->ref_count = 0;
    
    return node;
}

// Helper: Free a node and all its children
static void testfs_free_node(testfs_node_t* node) {
    if (node == NULL) {
        return;
    }
    
    // Free all children for directories
    if (node->type == NODE_TYPE_DIR) {
        testfs_node_t* child = node->first_child;
        while (child != NULL) {
            testfs_node_t* next = child->next_sibling;
            testfs_free_node(child);
            child = next;
        }
    }
    
    // Free file data
    if (node->data != NULL) {
        Dmod_Free(node->data);
    }
    
    Dmod_Free(node);
}

// Helper: Find a child by name
static testfs_node_t* testfs_find_child(testfs_node_t* parent, const char* name) {
    if (parent == NULL || parent->type != NODE_TYPE_DIR) {
        return NULL;
    }
    
    testfs_node_t* child = parent->first_child;
    while (child != NULL) {
        if (testfs_strcmp(child->name, name) == 0) {
            return child;
        }
        child = child->next_sibling;
    }
    return NULL;
}

// Helper: Add a child to a directory
static int testfs_add_child(testfs_node_t* parent, testfs_node_t* child) {
    if (parent == NULL || child == NULL || parent->type != NODE_TYPE_DIR) {
        return DMFSI_ERR_INVALID;
    }
    
    // Check if child with same name already exists
    if (testfs_find_child(parent, child->name) != NULL) {
        return DMFSI_ERR_EXISTS;
    }
    
    child->parent = parent;
    child->next_sibling = parent->first_child;
    parent->first_child = child;
    
    return DMFSI_OK;
}

// Helper: Remove a child from a directory
static int testfs_remove_child(testfs_node_t* parent, testfs_node_t* child) {
    if (parent == NULL || child == NULL || parent->type != NODE_TYPE_DIR) {
        return DMFSI_ERR_INVALID;
    }
    
    testfs_node_t* prev = NULL;
    testfs_node_t* curr = parent->first_child;
    
    while (curr != NULL) {
        if (curr == child) {
            if (prev == NULL) {
                parent->first_child = curr->next_sibling;
            } else {
                prev->next_sibling = curr->next_sibling;
            }
            child->parent = NULL;
            child->next_sibling = NULL;
            return DMFSI_OK;
        }
        prev = curr;
        curr = curr->next_sibling;
    }
    
    return DMFSI_ERR_NOT_FOUND;
}

// Helper: Resolve path to a node
static testfs_node_t* testfs_resolve_path(dmfsi_context_t ctx, const char* path) {
    if (ctx == NULL || ctx->magic != TESTFS_CONTEXT_MAGIC || path == NULL) {
        return NULL;
    }
    
    // Handle root path
    if (testfs_strcmp(path, "/") == 0) {
        return (testfs_node_t*)ctx->root_dir;
    }
    
    // Skip leading slash
    if (path[0] == '/') {
        path++;
    }
    
    // Start from root
    testfs_node_t* current = (testfs_node_t*)ctx->root_dir;
    
    // Parse path components
    char component[TESTFS_MAX_FILENAME];
    int comp_idx = 0;
    
    while (*path != '\0') {
        if (*path == '/') {
            if (comp_idx > 0) {
                component[comp_idx] = '\0';
                current = testfs_find_child(current, component);
                if (current == NULL || current->type != NODE_TYPE_DIR) {
                    return NULL;
                }
                comp_idx = 0;
            }
            path++;
        } else {
            if (comp_idx < TESTFS_MAX_FILENAME - 1) {
                component[comp_idx++] = *path;
            }
            path++;
        }
    }
    
    // Handle last component
    if (comp_idx > 0) {
        component[comp_idx] = '\0';
        current = testfs_find_child(current, component);
    }
    
    return current;
}

// Helper: Get parent path and filename from path
static int testfs_split_path(const char* path, char* parent_path, char* filename) {
    if (path == NULL || parent_path == NULL || filename == NULL) {
        return DMFSI_ERR_INVALID;
    }
    
    // Find last slash
    const char* last_slash = NULL;
    const char* p = path;
    while (*p != '\0') {
        if (*p == '/') {
            last_slash = p;
        }
        p++;
    }
    
    if (last_slash == NULL) {
        // No slash - invalid path
        return DMFSI_ERR_INVALID;
    }
    
    // Extract parent path
    if (last_slash == path) {
        // Parent is root
        parent_path[0] = '/';
        parent_path[1] = '\0';
    } else {
        size_t len = last_slash - path;
        if (len >= TESTFS_MAX_PATH) {
            return DMFSI_ERR_INVALID;
        }
        testfs_memcpy(parent_path, path, len);
        parent_path[len] = '\0';
    }
    
    // Extract filename
    testfs_strncpy(filename, last_slash + 1, TESTFS_MAX_FILENAME - 1);
    filename[TESTFS_MAX_FILENAME - 1] = '\0';
    
    return DMFSI_OK;
}

// Implement _init for TestFS
dmod_dmfsi_dif_api_declaration( 1.0, testfs, dmfsi_context_t, _init, (const char* config) )
{
    Dmod_Printf("TestFS: Initializing file system\n");
    
    // Allocate context
    struct dmfsi_context* ctx = (struct dmfsi_context*)Dmod_Malloc(sizeof(struct dmfsi_context));
    if (ctx == NULL) {
        Dmod_Printf("TestFS: Failed to allocate context\n");
        return NULL;
    }
    
    ctx->magic = TESTFS_CONTEXT_MAGIC;
    ctx->initialized = 1;
    
    // Create root directory
    testfs_node_t* root = testfs_create_node("/", NODE_TYPE_DIR);
    if (root == NULL) {
        Dmod_Free(ctx);
        return NULL;
    }
    
    ctx->root_dir = root;
    
    Dmod_Printf("TestFS: Initialized successfully\n");
    return ctx;
}

// Implement _deinit for TestFS
dmod_dmfsi_dif_api_declaration( 1.0, testfs, int, _deinit, (dmfsi_context_t ctx) )
{
    if (ctx == NULL || ctx->magic != TESTFS_CONTEXT_MAGIC) {
        return DMFSI_ERR_INVALID;
    }
    
    Dmod_Printf("TestFS: Deinitializing file system\n");
    
    // Free root directory and all its contents
    testfs_free_node((testfs_node_t*)ctx->root_dir);
    
    ctx->magic = 0;
    ctx->initialized = 0;
    Dmod_Free(ctx);
    
    return DMFSI_OK;
}

// Implement _context_is_valid for TestFS
dmod_dmfsi_dif_api_declaration( 1.0, testfs, int, _context_is_valid, (dmfsi_context_t ctx) )
{
    return (ctx != NULL && ctx->magic == TESTFS_CONTEXT_MAGIC && ctx->initialized) ? 1 : 0;
}

// Implement _fopen for TestFS
dmod_dmfsi_dif_api_declaration( 1.0, testfs, int, _fopen, (dmfsi_context_t ctx, void** fp, const char* path, int mode, int attr) )
{
    if (ctx == NULL || ctx->magic != TESTFS_CONTEXT_MAGIC || fp == NULL || path == NULL) {
        return DMFSI_ERR_INVALID;
    }
    
    testfs_node_t* node = testfs_resolve_path(ctx, path);
    
    if (node != NULL) {
        // File exists
        if (node->type != NODE_TYPE_FILE) {
            return DMFSI_ERR_INVALID;
        }
        
        if (mode & DMFSI_O_TRUNC) {
            // Truncate file
            node->size = 0;
            node->position = 0;
        }
        
        node->ref_count++;
        *fp = node;
        return DMFSI_OK;
    }
    
    // File doesn't exist
    if (!(mode & DMFSI_O_CREAT)) {
        return DMFSI_ERR_NOT_FOUND;
    }
    
    // Create new file
    char parent_path[TESTFS_MAX_PATH];
    char filename[TESTFS_MAX_FILENAME];
    
    if (testfs_split_path(path, parent_path, filename) != DMFSI_OK) {
        return DMFSI_ERR_INVALID;
    }
    
    testfs_node_t* parent = testfs_resolve_path(ctx, parent_path);
    if (parent == NULL || parent->type != NODE_TYPE_DIR) {
        return DMFSI_ERR_NOT_FOUND;
    }
    
    testfs_node_t* new_file = testfs_create_node(filename, NODE_TYPE_FILE);
    if (new_file == NULL) {
        return DMFSI_ERR_NO_SPACE;
    }
    
    new_file->attr = attr;
    
    int result = testfs_add_child(parent, new_file);
    if (result != DMFSI_OK) {
        testfs_free_node(new_file);
        return result;
    }
    
    new_file->ref_count++;
    *fp = new_file;
    return DMFSI_OK;
}

// Implement _fclose for TestFS
dmod_dmfsi_dif_api_declaration( 1.0, testfs, int, _fclose, (dmfsi_context_t ctx, void* fp) )
{
    if (ctx == NULL || ctx->magic != TESTFS_CONTEXT_MAGIC || fp == NULL) {
        return DMFSI_ERR_INVALID;
    }
    
    testfs_node_t* node = (testfs_node_t*)fp;
    if (node->ref_count > 0) {
        node->ref_count--;
    }
    
    return DMFSI_OK;
}

// Implement _fread for TestFS
dmod_dmfsi_dif_api_declaration( 1.0, testfs, int, _fread, (dmfsi_context_t ctx, void* fp, void* buffer, size_t size, size_t* read) )
{
    if (ctx == NULL || ctx->magic != TESTFS_CONTEXT_MAGIC || fp == NULL || buffer == NULL || read == NULL) {
        return DMFSI_ERR_INVALID;
    }
    
    testfs_node_t* node = (testfs_node_t*)fp;
    if (node->type != NODE_TYPE_FILE) {
        return DMFSI_ERR_INVALID;
    }
    
    size_t bytes_to_read = size;
    if (node->position + bytes_to_read > node->size) {
        bytes_to_read = node->size - node->position;
    }
    
    if (bytes_to_read > 0 && node->data != NULL) {
        testfs_memcpy(buffer, node->data + node->position, bytes_to_read);
        node->position += bytes_to_read;
    }
    
    *read = bytes_to_read;
    return DMFSI_OK;
}

// Implement _fwrite for TestFS
dmod_dmfsi_dif_api_declaration( 1.0, testfs, int, _fwrite, (dmfsi_context_t ctx, void* fp, const void* buffer, size_t size, size_t* written) )
{
    if (ctx == NULL || ctx->magic != TESTFS_CONTEXT_MAGIC || fp == NULL || buffer == NULL || written == NULL) {
        return DMFSI_ERR_INVALID;
    }
    
    testfs_node_t* node = (testfs_node_t*)fp;
    if (node->type != NODE_TYPE_FILE) {
        return DMFSI_ERR_INVALID;
    }
    
    // Check if we need to expand the buffer
    size_t required_capacity = node->position + size;
    if (required_capacity > node->capacity) {
        size_t new_capacity = required_capacity * 2;
        if (new_capacity < 1024) {
            new_capacity = 1024;
        }
        
        uint8_t* new_data = (uint8_t*)Dmod_Malloc(new_capacity);
        if (new_data == NULL) {
            return DMFSI_ERR_NO_SPACE;
        }
        
        if (node->data != NULL) {
            testfs_memcpy(new_data, node->data, node->size);
            Dmod_Free(node->data);
        }
        
        node->data = new_data;
        node->capacity = new_capacity;
    }
    
    testfs_memcpy(node->data + node->position, buffer, size);
    node->position += size;
    
    if (node->position > node->size) {
        node->size = node->position;
    }
    
    *written = size;
    return DMFSI_OK;
}

// Implement _lseek for TestFS
dmod_dmfsi_dif_api_declaration( 1.0, testfs, long, _lseek, (dmfsi_context_t ctx, void* fp, long offset, int whence) )
{
    if (ctx == NULL || ctx->magic != TESTFS_CONTEXT_MAGIC || fp == NULL) {
        return DMFSI_ERR_INVALID;
    }
    
    testfs_node_t* node = (testfs_node_t*)fp;
    if (node->type != NODE_TYPE_FILE) {
        return DMFSI_ERR_INVALID;
    }
    
    long new_position = 0;
    
    switch (whence) {
        case DMFSI_SEEK_SET:
            new_position = offset;
            break;
        case DMFSI_SEEK_CUR:
            new_position = (long)node->position + offset;
            break;
        case DMFSI_SEEK_END:
            new_position = (long)node->size + offset;
            break;
        default:
            return DMFSI_ERR_INVALID;
    }
    
    if (new_position < 0) {
        new_position = 0;
    }
    
    node->position = (size_t)new_position;
    return new_position;
}

// Implement _ioctl for TestFS
dmod_dmfsi_dif_api_declaration( 1.0, testfs, int, _ioctl, (dmfsi_context_t ctx, void* fp, int request, void* arg) )
{
    if (ctx == NULL || ctx->magic != TESTFS_CONTEXT_MAGIC) {
        return DMFSI_ERR_INVALID;
    }
    
    // Not implemented - return OK
    return DMFSI_OK;
}

// Implement _sync for TestFS
dmod_dmfsi_dif_api_declaration( 1.0, testfs, int, _sync, (dmfsi_context_t ctx, void* fp) )
{
    if (ctx == NULL || ctx->magic != TESTFS_CONTEXT_MAGIC) {
        return DMFSI_ERR_INVALID;
    }
    
    // In-memory FS - nothing to sync
    return DMFSI_OK;
}

// Implement _getc for TestFS
dmod_dmfsi_dif_api_declaration( 1.0, testfs, int, _getc, (dmfsi_context_t ctx, void* fp) )
{
    if (ctx == NULL || ctx->magic != TESTFS_CONTEXT_MAGIC || fp == NULL) {
        return DMFSI_ERR_INVALID;
    }
    
    testfs_node_t* node = (testfs_node_t*)fp;
    if (node->type != NODE_TYPE_FILE) {
        return DMFSI_ERR_INVALID;
    }
    
    if (node->position >= node->size) {
        return -1;  // EOF
    }
    
    int c = (int)node->data[node->position];
    node->position++;
    return c;
}

// Implement _putc for TestFS
dmod_dmfsi_dif_api_declaration( 1.0, testfs, int, _putc, (dmfsi_context_t ctx, void* fp, int c) )
{
    if (ctx == NULL || ctx->magic != TESTFS_CONTEXT_MAGIC || fp == NULL) {
        return DMFSI_ERR_INVALID;
    }
    
    uint8_t byte = (uint8_t)c;
    size_t written;
    
    int result = dmfsi_testfs_fwrite(ctx, fp, &byte, 1, &written);
    if (result != DMFSI_OK) {
        return result;
    }
    
    return c;
}

// Implement _tell for TestFS
dmod_dmfsi_dif_api_declaration( 1.0, testfs, long, _tell, (dmfsi_context_t ctx, void* fp) )
{
    if (ctx == NULL || ctx->magic != TESTFS_CONTEXT_MAGIC || fp == NULL) {
        return DMFSI_ERR_INVALID;
    }
    
    testfs_node_t* node = (testfs_node_t*)fp;
    if (node->type != NODE_TYPE_FILE) {
        return DMFSI_ERR_INVALID;
    }
    
    return (long)node->position;
}

// Implement _eof for TestFS
dmod_dmfsi_dif_api_declaration( 1.0, testfs, int, _eof, (dmfsi_context_t ctx, void* fp) )
{
    if (ctx == NULL || ctx->magic != TESTFS_CONTEXT_MAGIC || fp == NULL) {
        return DMFSI_ERR_INVALID;
    }
    
    testfs_node_t* node = (testfs_node_t*)fp;
    if (node->type != NODE_TYPE_FILE) {
        return DMFSI_ERR_INVALID;
    }
    
    return (node->position >= node->size) ? 1 : 0;
}

// Implement _size for TestFS
dmod_dmfsi_dif_api_declaration( 1.0, testfs, long, _size, (dmfsi_context_t ctx, void* fp) )
{
    if (ctx == NULL || ctx->magic != TESTFS_CONTEXT_MAGIC || fp == NULL) {
        return DMFSI_ERR_INVALID;
    }
    
    testfs_node_t* node = (testfs_node_t*)fp;
    if (node->type != NODE_TYPE_FILE) {
        return DMFSI_ERR_INVALID;
    }
    
    return (long)node->size;
}

// Implement _fflush for TestFS
dmod_dmfsi_dif_api_declaration( 1.0, testfs, int, _fflush, (dmfsi_context_t ctx, void* fp) )
{
    if (ctx == NULL || ctx->magic != TESTFS_CONTEXT_MAGIC) {
        return DMFSI_ERR_INVALID;
    }
    
    // In-memory FS - nothing to flush
    return DMFSI_OK;
}

// Implement _error for TestFS
dmod_dmfsi_dif_api_declaration( 1.0, testfs, int, _error, (dmfsi_context_t ctx, void* fp) )
{
    if (ctx == NULL || ctx->magic != TESTFS_CONTEXT_MAGIC) {
        return DMFSI_ERR_INVALID;
    }
    
    // No error tracking in this simple implementation
    return 0;
}

// Implement _opendir for TestFS
dmod_dmfsi_dif_api_declaration( 1.0, testfs, int, _opendir, (dmfsi_context_t ctx, void** dp, const char* path) )
{
    if (ctx == NULL || ctx->magic != TESTFS_CONTEXT_MAGIC || dp == NULL || path == NULL) {
        return DMFSI_ERR_INVALID;
    }
    
    testfs_node_t* node = testfs_resolve_path(ctx, path);
    if (node == NULL) {
        return DMFSI_ERR_NOT_FOUND;
    }
    
    if (node->type != NODE_TYPE_DIR) {
        return DMFSI_ERR_INVALID;
    }
    
    testfs_dir_iter_t* iter = (testfs_dir_iter_t*)Dmod_Malloc(sizeof(testfs_dir_iter_t));
    if (iter == NULL) {
        return DMFSI_ERR_NO_SPACE;
    }
    
    iter->dir = node;
    iter->current = node->first_child;
    node->ref_count++;
    
    *dp = iter;
    return DMFSI_OK;
}

// Implement _closedir for TestFS
dmod_dmfsi_dif_api_declaration( 1.0, testfs, int, _closedir, (dmfsi_context_t ctx, void* dp) )
{
    if (ctx == NULL || ctx->magic != TESTFS_CONTEXT_MAGIC || dp == NULL) {
        return DMFSI_ERR_INVALID;
    }
    
    testfs_dir_iter_t* iter = (testfs_dir_iter_t*)dp;
    if (iter->dir->ref_count > 0) {
        iter->dir->ref_count--;
    }
    
    Dmod_Free(iter);
    return DMFSI_OK;
}

// Implement _readdir for TestFS
dmod_dmfsi_dif_api_declaration( 1.0, testfs, int, _readdir, (dmfsi_context_t ctx, void* dp, dmfsi_dir_entry_t* entry) )
{
    if (ctx == NULL || ctx->magic != TESTFS_CONTEXT_MAGIC || dp == NULL || entry == NULL) {
        return DMFSI_ERR_INVALID;
    }
    
    testfs_dir_iter_t* iter = (testfs_dir_iter_t*)dp;
    
    if (iter->current == NULL) {
        return DMFSI_ERR_NOT_FOUND;  // End of directory
    }
    
    testfs_strncpy(entry->name, iter->current->name, 255);
    entry->name[255] = '\0';
    entry->size = (uint32_t)iter->current->size;
    entry->attr = iter->current->attr;
    entry->time = iter->current->mtime;
    
    iter->current = iter->current->next_sibling;
    
    return DMFSI_OK;
}

// Implement _stat for TestFS
dmod_dmfsi_dif_api_declaration( 1.0, testfs, int, _stat, (dmfsi_context_t ctx, const char* path, dmfsi_stat_t* stat) )
{
    if (ctx == NULL || ctx->magic != TESTFS_CONTEXT_MAGIC || path == NULL || stat == NULL) {
        return DMFSI_ERR_INVALID;
    }
    
    testfs_node_t* node = testfs_resolve_path(ctx, path);
    if (node == NULL) {
        return DMFSI_ERR_NOT_FOUND;
    }
    
    stat->size = (uint32_t)node->size;
    stat->attr = node->attr;
    stat->ctime = node->ctime;
    stat->mtime = node->mtime;
    stat->atime = node->atime;
    
    return DMFSI_OK;
}

// Implement _unlink for TestFS
dmod_dmfsi_dif_api_declaration( 1.0, testfs, int, _unlink, (dmfsi_context_t ctx, const char* path) )
{
    if (ctx == NULL || ctx->magic != TESTFS_CONTEXT_MAGIC || path == NULL) {
        return DMFSI_ERR_INVALID;
    }
    
    testfs_node_t* node = testfs_resolve_path(ctx, path);
    if (node == NULL) {
        return DMFSI_ERR_NOT_FOUND;
    }
    
    if (node->type != NODE_TYPE_FILE) {
        return DMFSI_ERR_INVALID;
    }
    
    if (node->ref_count > 0) {
        return DMFSI_ERR_INVALID;  // File is open
    }
    
    testfs_node_t* parent = node->parent;
    if (parent != NULL) {
        testfs_remove_child(parent, node);
    }
    
    testfs_free_node(node);
    return DMFSI_OK;
}

// Implement _rename for TestFS
dmod_dmfsi_dif_api_declaration( 1.0, testfs, int, _rename, (dmfsi_context_t ctx, const char* oldpath, const char* newpath) )
{
    if (ctx == NULL || ctx->magic != TESTFS_CONTEXT_MAGIC || oldpath == NULL || newpath == NULL) {
        return DMFSI_ERR_INVALID;
    }
    
    testfs_node_t* node = testfs_resolve_path(ctx, oldpath);
    if (node == NULL) {
        return DMFSI_ERR_NOT_FOUND;
    }
    
    // Check if new path already exists
    if (testfs_resolve_path(ctx, newpath) != NULL) {
        return DMFSI_ERR_EXISTS;
    }
    
    // Extract new parent and filename
    char new_parent_path[TESTFS_MAX_PATH];
    char new_filename[TESTFS_MAX_FILENAME];
    
    if (testfs_split_path(newpath, new_parent_path, new_filename) != DMFSI_OK) {
        return DMFSI_ERR_INVALID;
    }
    
    testfs_node_t* new_parent = testfs_resolve_path(ctx, new_parent_path);
    if (new_parent == NULL || new_parent->type != NODE_TYPE_DIR) {
        return DMFSI_ERR_NOT_FOUND;
    }
    
    // Remove from old parent
    testfs_node_t* old_parent = node->parent;
    if (old_parent != NULL) {
        testfs_remove_child(old_parent, node);
    }
    
    // Update name and add to new parent
    testfs_strncpy(node->name, new_filename, TESTFS_MAX_FILENAME - 1);
    node->name[TESTFS_MAX_FILENAME - 1] = '\0';
    
    return testfs_add_child(new_parent, node);
}

// Implement _chmod for TestFS
dmod_dmfsi_dif_api_declaration( 1.0, testfs, int, _chmod, (dmfsi_context_t ctx, const char* path, int mode) )
{
    if (ctx == NULL || ctx->magic != TESTFS_CONTEXT_MAGIC || path == NULL) {
        return DMFSI_ERR_INVALID;
    }
    
    testfs_node_t* node = testfs_resolve_path(ctx, path);
    if (node == NULL) {
        return DMFSI_ERR_NOT_FOUND;
    }
    
    // Simple implementation - just store mode in attr
    // In a real FS, you'd have separate mode field
    return DMFSI_OK;
}

// Implement _utime for TestFS
dmod_dmfsi_dif_api_declaration( 1.0, testfs, int, _utime, (dmfsi_context_t ctx, const char* path, uint32_t atime, uint32_t mtime) )
{
    if (ctx == NULL || ctx->magic != TESTFS_CONTEXT_MAGIC || path == NULL) {
        return DMFSI_ERR_INVALID;
    }
    
    testfs_node_t* node = testfs_resolve_path(ctx, path);
    if (node == NULL) {
        return DMFSI_ERR_NOT_FOUND;
    }
    
    node->atime = atime;
    node->mtime = mtime;
    
    return DMFSI_OK;
}

// Implement _mkdir for TestFS
dmod_dmfsi_dif_api_declaration( 1.0, testfs, int, _mkdir, (dmfsi_context_t ctx, const char* path, int mode) )
{
    if (ctx == NULL || ctx->magic != TESTFS_CONTEXT_MAGIC || path == NULL) {
        return DMFSI_ERR_INVALID;
    }
    
    // Check if directory already exists
    if (testfs_resolve_path(ctx, path) != NULL) {
        return DMFSI_ERR_EXISTS;
    }
    
    // Extract parent and dirname
    char parent_path[TESTFS_MAX_PATH];
    char dirname[TESTFS_MAX_FILENAME];
    
    if (testfs_split_path(path, parent_path, dirname) != DMFSI_OK) {
        return DMFSI_ERR_INVALID;
    }
    
    testfs_node_t* parent = testfs_resolve_path(ctx, parent_path);
    if (parent == NULL || parent->type != NODE_TYPE_DIR) {
        return DMFSI_ERR_NOT_FOUND;
    }
    
    testfs_node_t* new_dir = testfs_create_node(dirname, NODE_TYPE_DIR);
    if (new_dir == NULL) {
        return DMFSI_ERR_NO_SPACE;
    }
    
    int result = testfs_add_child(parent, new_dir);
    if (result != DMFSI_OK) {
        testfs_free_node(new_dir);
        return result;
    }
    
    return DMFSI_OK;
}

// Implement _direxists for TestFS
dmod_dmfsi_dif_api_declaration( 1.0, testfs, int, _direxists, (dmfsi_context_t ctx, const char* path) )
{
    if (ctx == NULL || ctx->magic != TESTFS_CONTEXT_MAGIC || path == NULL) {
        return 0;
    }
    
    testfs_node_t* node = testfs_resolve_path(ctx, path);
    return (node != NULL && node->type == NODE_TYPE_DIR) ? 1 : 0;
}

// Module init/deinit
int dmod_init(const Dmod_Config_t *Config)
{
    Dmod_Printf("TestFS module initialized\n");
    return 0;
}

int dmod_deinit(void)
{
    Dmod_Printf("TestFS module deinitialized\n");
    return 0;
}
