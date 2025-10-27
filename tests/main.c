#include <stdio.h>
#include <string.h>
#include "dmod.h"
#include "dmvfs.h"
#include "test_framework.h"

// Force testfs module symbols to be linked
extern int dmod_init(const Dmod_Config_t *Config);
extern int dmod_deinit(void);

static void __attribute__((used)) force_testfs_link(void) {
    // Reference dmod_init and dmod_deinit to ensure testfs is linked
    (void)dmod_init;
    (void)dmod_deinit;
}

// -----------------------------------------
//
//      Prints usage message
//
// -----------------------------------------
void PrintUsage( const char* AppName )
{
    printf("Usage: %s\n", AppName);
    printf("Run DMVFS API tests with the built-in testfs module\n");
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
    printf("using the built-in testfs module (statically linked).\n\n");
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
    // Help and version options
    if( argc >= 2 )
    {
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
    }

    printf("DMVFS Test Suite\n");
    printf("================\n\n");

    // Initialize DMVFS
    if (!dmvfs_init( 16, 32 ))
    {
        printf("Cannot initialize DMVFS\n");
        return -1;
    }

    printf("DMVFS initialized successfully.\nMounting testfs at /mnt...\n");

    // Mount testfs (statically linked module)
    if(!dmvfs_mount_fs( "testfs", "/mnt", NULL ))
    {
        printf("Cannot mount testfs at /mnt\n");
        printf("Note: testfs module is statically linked\n");
        dmvfs_deinit();
        return -1;
    }

    printf("testfs mounted at /mnt successfully.\n");

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

