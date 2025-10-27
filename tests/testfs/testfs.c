#define DMOD_ENABLE_REGISTRATION    ON
#define ENABLE_DIF_REGISTRATIONS    ON
#include "dmod.h"
#include "dmfsi.h"

/**
 * @brief Pre-initialization function for the module.
 * 
 * @note This function is optional. You can remove it if you don't need it.
 * 
 * This function is called when the module enabling is in progress.
 * 
 * You can use this function to load the required dependencies, such as 
 * other modules. Please be aware that the module is not fully initialized, 
 * so not all the API functions are available - you can check if the API
 * is connected by calling the Dmod_IsFunctionConnected() function.
 */
void dmod_preinit(void)
{
    if(Dmod_IsFunctionConnected( Dmod_Printf ))
    {
        Dmod_Printf("API is connected!\n");
    }
}

/**
 * @brief Initialization function for the module.
 * 
 * This function is called when the module is enabled.
 * Please use this function to initialize the module, for instance:
 * - initialize the module variables
 * - initialize the module hardware
 * - allocate memory
 */
int dmod_init(const Dmod_Config_t *Config)
{
    Dmod_Printf("testfs initialized\n");
    return 0;
}

/**
 * @brief De-initialization function for the module.
 * 
 * This function is called when the module is disabled.
 * Please use this function to de-initialize the module, for instance:
 * - free memory
 * - de-initialize the module hardware
 * - de-initialize the module variables
 */
int dmod_deinit(void)
{
    Dmod_Printf("testfs deinitialized!\n");
    return 0;
}

dmod_dmfsi_dif_api_declaration( 1.0, testfs, dmfsi_context_t, _init, (const char* config) )
{
    return NULL;
}

dmod_dmfsi_dif_api_declaration( 1.0, testfs, int, _deinit, (dmfsi_context_t ctx) )
{
    return 0;
}

dmod_dmfsi_dif_api_declaration( 1.0, testfs, int, _context_is_valid, (dmfsi_context_t ctx) )
{
    return 1;
}

dmod_dmfsi_dif_api_declaration( 1.0, testfs, int, _fopen, (dmfsi_context_t ctx, void** fp, const char* path, int mode, int attr) )
{
    return 0;
}

dmod_dmfsi_dif_api_declaration( 1.0, testfs, int, _fclose, (dmfsi_context_t ctx, void* fp) )
{
    return 0;
}

dmod_dmfsi_dif_api_declaration( 1.0, testfs, int, _fread, (dmfsi_context_t ctx, void* fp, void* buffer, size_t size, size_t* read) )
{
    return 0;
}

dmod_dmfsi_dif_api_declaration( 1.0, testfs, int, _fwrite, (dmfsi_context_t ctx, void* fp, const void* buffer, size_t size, size_t* written) )
{
    return 0;
}

dmod_dmfsi_dif_api_declaration( 1.0, testfs, long, _lseek, (dmfsi_context_t ctx, void* fp, long offset, int whence) )
{
    return 0;
}

dmod_dmfsi_dif_api_declaration( 1.0, testfs, int, _ioctl, (dmfsi_context_t ctx, void* fp, int request, void* arg) )
{
    return 0;
}

dmod_dmfsi_dif_api_declaration( 1.0, testfs, int, _sync, (dmfsi_context_t ctx, void* fp) )
{
    return 0;
}

dmod_dmfsi_dif_api_declaration( 1.0, testfs, int, _getc, (dmfsi_context_t ctx, void* fp) )
{
    return 0;
}

dmod_dmfsi_dif_api_declaration( 1.0, testfs, int, _putc, (dmfsi_context_t ctx, void* fp, int c) )
{
    return 0;
}

dmod_dmfsi_dif_api_declaration( 1.0, testfs, long, _tell, (dmfsi_context_t ctx, void* fp) )
{
    return 0;
}

dmod_dmfsi_dif_api_declaration( 1.0, testfs, int, _eof, (dmfsi_context_t ctx, void* fp) )
{
    return 0;
}

dmod_dmfsi_dif_api_declaration( 1.0, testfs, long, _size, (dmfsi_context_t ctx, void* fp) )
{
    return 0;
}

dmod_dmfsi_dif_api_declaration( 1.0, testfs, int, _fflush, (dmfsi_context_t ctx, void* fp) )
{
    return 0;
}

dmod_dmfsi_dif_api_declaration( 1.0, testfs, int, _error, (dmfsi_context_t ctx, void* fp) )
{
    return 0;
}

dmod_dmfsi_dif_api_declaration( 1.0, testfs, int, _opendir, (dmfsi_context_t ctx, void** dp, const char* path) )
{
    return 0;
}

dmod_dmfsi_dif_api_declaration( 1.0, testfs, int, _closedir, (dmfsi_context_t ctx, void* dp) )
{
    return 0;
}

dmod_dmfsi_dif_api_declaration( 1.0, testfs, int, _readdir, (dmfsi_context_t ctx, void* dp, dmfsi_dir_entry_t* entry) )
{
    return 0;
}

dmod_dmfsi_dif_api_declaration( 1.0, testfs, int, _stat, (dmfsi_context_t ctx, const char* path, dmfsi_stat_t* stat) )
{
    return 0;
}

dmod_dmfsi_dif_api_declaration( 1.0, testfs, int, _unlink, (dmfsi_context_t ctx, const char* path) )
{
    return 0;
}

dmod_dmfsi_dif_api_declaration( 1.0, testfs, int, _rename, (dmfsi_context_t ctx, const char* oldpath, const char* newpath) )
{
    return 0;
}

dmod_dmfsi_dif_api_declaration( 1.0, testfs, int, _chmod, (dmfsi_context_t ctx, const char* path, int mode) )
{
    return 0;
}

dmod_dmfsi_dif_api_declaration( 1.0, testfs, int, _utime, (dmfsi_context_t ctx, const char* path, uint32_t atime, uint32_t mtime) )
{
    return 0;
}

dmod_dmfsi_dif_api_declaration( 1.0, testfs, int, _mkdir, (dmfsi_context_t ctx, const char* path, int mode) )
{
    return 0;
}

dmod_dmfsi_dif_api_declaration( 1.0, testfs, int, _direxists, (dmfsi_context_t ctx, const char* path) )
{
    return 0;
}
