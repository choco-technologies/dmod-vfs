#include <stdio.h>
#include <string.h>
#include "dmod.h"
#include "dmvfs.h"
#include "test_framework.h"

// -----------------------------------------
//
//      Prints usage message
//
// -----------------------------------------
void PrintUsage( const char* AppName )
{
    printf("Usage: %s path/to/ramfs.dmf\n", AppName);
    printf("Run DMVFS API tests with the specified ramfs module\n");
    printf("Options:\n");
    printf("  -h, --help     Print help message\n");
    printf("  -v, --version  Print version information\n");
}

// -----------------------------------------
//
//      Prints help message
//
// -----------------------------------------
void PrintHelp( const char* AppName )
{
    printf("-- Dynamic Module Loader ver. " DMOD_VERSION_STRING " --\n\n");
    printf("DMVFS API Test Suite\n");
    printf("This application tests the DMOD Virtual File System API\n");
    printf("using a ramfs module loaded at runtime.\n\n");
    PrintUsage( AppName );
}

// -----------------------------------------
//
//      Run all test suites
//
// -----------------------------------------
void RunAllTests(void)
{
    // Initialize test framework
    test_framework_init();
    
    printf("\n");
    printf("=========================================\n");
    printf("  DMVFS API Test Suite\n");
    printf("=========================================\n");
    
    // Run file operation tests
    printf("\n--- FILE OPERATION TESTS ---\n");
    for (int i = 0; file_operation_tests[i].name != NULL; i++) {
        file_operation_tests[i].function();
    }
    
    // Run directory operation tests
    printf("\n--- DIRECTORY OPERATION TESTS ---\n");
    for (int i = 0; directory_operation_tests[i].name != NULL; i++) {
        directory_operation_tests[i].function();
    }
    
    // Run path operation tests
    printf("\n--- PATH OPERATION TESTS ---\n");
    for (int i = 0; path_operation_tests[i].name != NULL; i++) {
        path_operation_tests[i].function();
    }
    
    // Print summary
    test_framework_print_summary();
}

// -----------------------------------------
//
//      Main function
//
// -----------------------------------------
int main( int argc, char *argv[] )
{
    // Check for minimum arguments
    if( argc < 2 )
    {
        PrintUsage( argv[0] );
        return 0;
    }
    
    // Help and version options
    if( strcmp( argv[1], "-h" ) == 0 || strcmp( argv[1], "--help" ) == 0 )
    {
        PrintHelp( argv[0] );
        return 0;
    }

    if( strcmp( argv[1], "-v" ) == 0 || strcmp( argv[1], "--version" ) == 0 )
    {
        printf("Dynamic Module Loader ver. " DMOD_VERSION_STRING "\n");
        return 0;
    }

    printf("DMVFS Test Suite\n");
    printf("================\n\n");

    // Load ramfs module from file
    Dmod_Context_t* context = Dmod_LoadFile( argv[1] );
    if( context == NULL )
    {
        printf("Cannot load module: %s\n", argv[1]);
        return -1;
    }

    if (!Dmod_Enable( context, false, NULL ))
    {
        printf("Cannot enable module: %s\n", argv[1]);
        Dmod_Unload( context, false );
        return -1;
    }

    printf("Module '%s' loaded and enabled successfully.\n", Dmod_GetName( context ));

    // Initialize DMVFS
    if (!dmvfs_init( 16, 32 ))
    {
        printf("Cannot initialize DMVFS\n");
        return -1;
    }

    printf("DMVFS initialized successfully.\nMounting ramfs at /mnt...\n");

    if(!dmvfs_mount_fs( "ramfs", "/mnt", NULL ))
    {
        printf("Cannot mount ramfs at /mnt\n");
        dmvfs_deinit();
        return -1;
    }

    printf("ramfs mounted at /mnt successfully.\n");

    // Run all tests
    RunAllTests();

    // Cleanup
    if(!dmvfs_unmount_fs( "/mnt" ))
    {
        printf("Cannot unmount /mnt\n");
    }

    printf("\nUnmounted /mnt successfully.\nDeinitializing DMVFS...\n");

    dmvfs_deinit();

    return (g_test_stats.failed == 0) ? 0 : 1;
}

