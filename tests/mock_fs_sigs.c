#include "dmfsi.h"
#include <string.h>
#include <stdlib.h>

// Mock filesystem for testing - provides DIF signatures

// Define the DIF signatures that dmvfs needs
#define DMOD_ENABLE_REGISTRATION ON
#define DMOD_mockfs

// Include dmfsi to get the DIF signatures defined
#include "dmfsi_defs.h"

// Define all required DIF signatures as const strings
const char* const dmod_dmfsi_init_sig = "dmfsi:1.0:_init";
const char* const dmod_dmfsi_deinit_sig = "dmfsi:1.0:_deinit";
const char* const dmod_dmfsi_fopen_sig = "dmfsi:1.0:_fopen";
const char* const dmod_dmfsi_fclose_sig = "dmfsi:1.0:_fclose";
const char* const dmod_dmfsi_fread_sig = "dmfsi:1.0:_fread";
const char* const dmod_dmfsi_fwrite_sig = "dmfsi:1.0:_fwrite";
const char* const dmod_dmfsi_lseek_sig = "dmfsi:1.0:_lseek";
const char* const dmod_dmfsi_tell_sig = "dmfsi:1.0:_tell";
const char* const dmod_dmfsi_eof_sig = "dmfsi:1.0:_eof";
const char* const dmod_dmfsi_fflush_sig = "dmfsi:1.0:_fflush";
const char* const dmod_dmfsi_error_sig = "dmfsi:1.0:_error";
const char* const dmod_dmfsi_unlink_sig = "dmfsi:1.0:_unlink";
const char* const dmod_dmfsi_rename_sig = "dmfsi:1.0:_rename";
const char* const dmod_dmfsi_ioctl_sig = "dmfsi:1.0:_ioctl";
const char* const dmod_dmfsi_sync_sig = "dmfsi:1.0:_sync";
const char* const dmod_dmfsi_stat_sig = "dmfsi:1.0:_stat";
const char* const dmod_dmfsi_getc_sig = "dmfsi:1.0:_getc";
const char* const dmod_dmfsi_putc_sig = "dmfsi:1.0:_putc";
const char* const dmod_dmfsi_chmod_sig = "dmfsi:1.0:_chmod";
const char* const dmod_dmfsi_utime_sig = "dmfsi:1.0:_utime";
const char* const dmod_dmfsi_mkdir_sig = "dmfsi:1.0:_mkdir";
const char* const dmod_dmfsi_direxists_sig = "dmfsi:1.0:_direxists";
const char* const dmod_dmfsi_opendir_sig = "dmfsi:1.0:_opendir";
const char* const dmod_dmfsi_readdir_sig = "dmfsi:1.0:_readdir";
const char* const dmod_dmfsi_closedir_sig = "dmfsi:1.0:_closedir";
