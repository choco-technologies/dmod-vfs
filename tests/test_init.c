#include "test_framework.h"
#include "dmvfs.h"
#include <string.h>

// Test initialization with valid parameters
void test_init_valid(void)
{
    bool result = dmvfs_init(10, 20);
    TEST_ASSERT(result == true, "Initialize DMVFS with valid parameters");
    
    // Clean up
    dmvfs_deinit();
}

// Test initialization with invalid parameters
void test_init_invalid_mount_points(void)
{
    bool result = dmvfs_init(0, 10);
    TEST_ASSERT(result == false, "Initialize DMVFS with zero mount points should fail");
    
    result = dmvfs_init(-1, 10);
    TEST_ASSERT(result == false, "Initialize DMVFS with negative mount points should fail");
}

// Test double initialization
void test_double_init(void)
{
    bool result = dmvfs_init(5, 10);
    TEST_ASSERT(result == true, "First initialization should succeed");
    
    result = dmvfs_init(5, 10);
    TEST_ASSERT(result == false, "Second initialization should fail");
    
    // Clean up
    dmvfs_deinit();
}

// Test deinitialization
void test_deinit(void)
{
    // Initialize first
    bool result = dmvfs_init(5, 10);
    TEST_ASSERT(result == true, "Initialize before deinit");
    
    // Deinitialize
    result = dmvfs_deinit();
    TEST_ASSERT(result == true, "Deinitialize DMVFS");
}

// Test deinit without init
void test_deinit_without_init(void)
{
    bool result = dmvfs_deinit();
    TEST_ASSERT(result == false, "Deinitialize without init should fail");
}

// Test get max mount points
void test_get_max_mount_points(void)
{
    int max_mount_points = 15;
    bool result = dmvfs_init(max_mount_points, 20);
    TEST_ASSERT(result == true, "Initialize DMVFS");
    
    int retrieved = dmvfs_get_max_mount_points();
    TEST_ASSERT_EQUAL(max_mount_points, retrieved, "Get max mount points");
    
    // Clean up
    dmvfs_deinit();
}

// Test get max open files
void test_get_max_open_files(void)
{
    int max_open_files = 30;
    bool result = dmvfs_init(10, max_open_files);
    TEST_ASSERT(result == true, "Initialize DMVFS");
    
    int retrieved = dmvfs_get_max_open_files();
    TEST_ASSERT_EQUAL(max_open_files, retrieved, "Get max open files");
    
    // Clean up
    dmvfs_deinit();
}

// Test get parameters before init
void test_get_params_before_init(void)
{
    int mount_points = dmvfs_get_max_mount_points();
    TEST_ASSERT_EQUAL(0, mount_points, "Get max mount points before init should return 0");
    
    int open_files = dmvfs_get_max_open_files();
    TEST_ASSERT_EQUAL(0, open_files, "Get max open files before init should return 0");
}

int main(void)
{
    printf("========================================\n");
    printf("DMVFS Initialization Tests\n");
    printf("========================================\n");
    
    RUN_TEST(test_init_valid);
    RUN_TEST(test_init_invalid_mount_points);
    RUN_TEST(test_double_init);
    RUN_TEST(test_deinit);
    RUN_TEST(test_deinit_without_init);
    RUN_TEST(test_get_max_mount_points);
    RUN_TEST(test_get_max_open_files);
    RUN_TEST(test_get_params_before_init);
    
    TEST_SUMMARY();
    return TEST_RETURN_CODE();
}
