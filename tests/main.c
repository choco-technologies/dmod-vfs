#include <stdio.h>
#include <string.h>
#include "dmod.h"
#include "dmvfs.h"

// -----------------------------------------
//
//      Prints usage message
//
// -----------------------------------------
void PrintUsage( const char* AppName )
{
    printf("Usage: %s path/to/file.dmf\n", AppName);
}

// -----------------------------------------
//
//      Prints help message
//
// -----------------------------------------
void PrintHelp( const char* AppName )
{
    printf("-- Dynamic Module Loader ver. " DMOD_VERSION_STRING " --\n\n");
    printf("The DMOD is a dynamic module loader that allows to load and unload modules\n");
    printf("This is an example application that uses the DMOD system\n\n");
    printf("Usage: %s path/to/file.dmf\n", AppName);
    printf("Options:\n");
    printf("  -h, --help     Print this help message\n");
    printf("  -v, --version  Print version information\n");
}

// -----------------------------------------
//
//      Main function
//
// -----------------------------------------
int main( int argc, char *argv[] )
{
    if( argc < 2 )
    {
        PrintUsage( argv[0] );
        return 0;
    }

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

    if (!dmvfs_init( 16, 32 ))
    {
        printf("Cannot initialize DMVFS\n");
        return -1;
    }

    printf("DMVFS initialized successfully.\n");

    return 0;
}

