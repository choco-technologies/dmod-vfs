#include "test_framework.h"
#include "dmvfs.h"
#include <string.h>

// Test mount_fs before init
void test_mount_fs_before_init(void)
{
    bool result = dmvfs_mount_fs("testfs", "/mnt", NULL);
    TEST_ASSERT(result == false, "Mount FS before init should fail");
}

// Test mount_fs with NULL fs_name
void test_mount_fs_null_name(void)
{
    bool result = dmvfs_init(5, 10);
    TEST_ASSERT(result == true, "Initialize DMVFS");
    
    result = dmvfs_mount_fs(NULL, "/mnt", NULL);
    TEST_ASSERT(result == false, "Mount FS with NULL name should fail");
    
    // Clean up
    dmvfs_deinit();
}

// Test mount_fs with NULL mount_point
void test_mount_fs_null_mount_point(void)
{
    bool result = dmvfs_init(5, 10);
    TEST_ASSERT(result == true, "Initialize DMVFS");
    
    result = dmvfs_mount_fs("testfs", NULL, NULL);
    TEST_ASSERT(result == false, "Mount FS with NULL mount point should fail");
    
    // Clean up
    dmvfs_deinit();
}

// Test mount_fs with non-existent filesystem
void test_mount_fs_nonexistent(void)
{
    bool result = dmvfs_init(5, 10);
    TEST_ASSERT(result == true, "Initialize DMVFS");
    
    // Try to mount a filesystem that doesn't exist
    result = dmvfs_mount_fs("nonexistent_fs", "/mnt", NULL);
    TEST_ASSERT(result == false, "Mount non-existent FS should fail");
    
    // Clean up
    dmvfs_deinit();
}

// Test unmount_fs before init
void test_unmount_fs_before_init(void)
{
    bool result = dmvfs_unmount_fs("/mnt");
    TEST_ASSERT(result == false, "Unmount FS before init should fail");
}

// Test unmount_fs with NULL mount_point
void test_unmount_fs_null_mount_point(void)
{
    bool result = dmvfs_init(5, 10);
    TEST_ASSERT(result == true, "Initialize DMVFS");
    
    result = dmvfs_unmount_fs(NULL);
    TEST_ASSERT(result == false, "Unmount FS with NULL mount point should fail");
    
    // Clean up
    dmvfs_deinit();
}

// Test unmount_fs non-existent mount point
void test_unmount_fs_nonexistent(void)
{
    bool result = dmvfs_init(5, 10);
    TEST_ASSERT(result == true, "Initialize DMVFS");
    
    result = dmvfs_unmount_fs("/nonexistent");
    TEST_ASSERT(result == false, "Unmount non-existent mount point should fail");
    
    // Clean up
    dmvfs_deinit();
}

int main(void)
{
    printf("========================================\n");
    printf("DMVFS Mount Operations Tests\n");
    printf("========================================\n");
    
    RUN_TEST(test_mount_fs_before_init);
    RUN_TEST(test_mount_fs_null_name);
    RUN_TEST(test_mount_fs_null_mount_point);
    RUN_TEST(test_mount_fs_nonexistent);
    RUN_TEST(test_unmount_fs_before_init);
    RUN_TEST(test_unmount_fs_null_mount_point);
    RUN_TEST(test_unmount_fs_nonexistent);
    
    TEST_SUMMARY();
    return TEST_RETURN_CODE();
}
