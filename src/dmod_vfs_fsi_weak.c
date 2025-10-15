/**
 * MIT License
 * 
 * Copyright (c) 2025 Choco-Technologies
 * 
 * @brief FSI DIF Signature Declarations
 * 
 * This file declares the FSI (File System Interface) DIF signatures as weak symbols.
 * These symbols will be provided by the dmod-fsi module if it is linked.
 * If not linked, these will be NULL and the VFS will still compile but won't be able
 * to mount file systems.
 * 
 * This approach allows the VFS library to be used standalone or with FSI modules.
 */

#include <stddef.h>

// Weak symbol declarations for FSI DIF signatures
// These will be NULL if fsi module is not linked
__attribute__((weak)) const char* const dmod_fsi_fopen_sig = NULL;
__attribute__((weak)) const char* const dmod_fsi_fclose_sig = NULL;
__attribute__((weak)) const char* const dmod_fsi_fread_sig = NULL;
__attribute__((weak)) const char* const dmod_fsi_fwrite_sig = NULL;
__attribute__((weak)) const char* const dmod_fsi_lseek_sig = NULL;
__attribute__((weak)) const char* const dmod_fsi_tell_sig = NULL;
__attribute__((weak)) const char* const dmod_fsi_size_sig = NULL;
__attribute__((weak)) const char* const dmod_fsi_stat_sig = NULL;

// Additional FSI signatures that might be needed
__attribute__((weak)) const char* const dmod_fsi_init_sig = NULL;
__attribute__((weak)) const char* const dmod_fsi_deinit_sig = NULL;
__attribute__((weak)) const char* const dmod_fsi_sync_sig = NULL;
__attribute__((weak)) const char* const dmod_fsi_eof_sig = NULL;
__attribute__((weak)) const char* const dmod_fsi_getc_sig = NULL;
__attribute__((weak)) const char* const dmod_fsi_putc_sig = NULL;
__attribute__((weak)) const char* const dmod_fsi_fflush_sig = NULL;
__attribute__((weak)) const char* const dmod_fsi_error_sig = NULL;
__attribute__((weak)) const char* const dmod_fsi_opendir_sig = NULL;
__attribute__((weak)) const char* const dmod_fsi_closedir_sig = NULL;
__attribute__((weak)) const char* const dmod_fsi_readdir_sig = NULL;
__attribute__((weak)) const char* const dmod_fsi_unlink_sig = NULL;
__attribute__((weak)) const char* const dmod_fsi_rename_sig = NULL;
__attribute__((weak)) const char* const dmod_fsi_chmod_sig = NULL;
__attribute__((weak)) const char* const dmod_fsi_utime_sig = NULL;
__attribute__((weak)) const char* const dmod_fsi_mkdir_sig = NULL;
__attribute__((weak)) const char* const dmod_fsi_direxists_sig = NULL;
