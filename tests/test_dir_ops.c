#include "test_framework.h"
#include "dmvfs.h"
#include <string.h>

// Test mkdir before init
void test_mkdir_before_init(void)
{
    int ret = dmvfs_mkdir("/testdir", 0755);
    TEST_ASSERT_EQUAL(-1, ret, "Create directory before init should fail");
}

// Test mkdir with NULL path
void test_mkdir_null_path(void)
{
    bool result = dmvfs_init(5, 10);
    TEST_ASSERT(result == true, "Initialize DMVFS");
    
    int ret = dmvfs_mkdir(NULL, 0755);
    TEST_ASSERT_EQUAL(-1, ret, "Create directory with NULL path should fail");
    
    // Clean up
    dmvfs_deinit();
}

// Test rmdir before init
void test_rmdir_before_init(void)
{
    int ret = dmvfs_rmdir("/testdir");
    TEST_ASSERT_EQUAL(-1, ret, "Remove directory before init should fail");
}

// Test rmdir with NULL path
void test_rmdir_null_path(void)
{
    bool result = dmvfs_init(5, 10);
    TEST_ASSERT(result == true, "Initialize DMVFS");
    
    int ret = dmvfs_rmdir(NULL);
    TEST_ASSERT_EQUAL(-1, ret, "Remove directory with NULL path should fail");
    
    // Clean up
    dmvfs_deinit();
}

// Test chdir before init
void test_chdir_before_init(void)
{
    int ret = dmvfs_chdir("/testdir");
    TEST_ASSERT_EQUAL(-1, ret, "Change directory before init should fail");
}

// Test chdir with NULL path
void test_chdir_null_path(void)
{
    bool result = dmvfs_init(5, 10);
    TEST_ASSERT(result == true, "Initialize DMVFS");
    
    int ret = dmvfs_chdir(NULL);
    TEST_ASSERT_EQUAL(-1, ret, "Change directory with NULL path should fail");
    
    // Clean up
    dmvfs_deinit();
}

// Test opendir before init
void test_opendir_before_init(void)
{
    void* dp = NULL;
    int ret = dmvfs_opendir(&dp, "/");
    TEST_ASSERT_EQUAL(-1, ret, "Open directory before init should fail");
}

// Test opendir with NULL pointer
void test_opendir_null_pointer(void)
{
    bool result = dmvfs_init(5, 10);
    TEST_ASSERT(result == true, "Initialize DMVFS");
    
    int ret = dmvfs_opendir(NULL, "/");
    TEST_ASSERT_EQUAL(-1, ret, "Open directory with NULL pointer should fail");
    
    // Clean up
    dmvfs_deinit();
}

// Test opendir with NULL path
void test_opendir_null_path(void)
{
    bool result = dmvfs_init(5, 10);
    TEST_ASSERT(result == true, "Initialize DMVFS");
    
    void* dp = NULL;
    int ret = dmvfs_opendir(&dp, NULL);
    TEST_ASSERT_EQUAL(-1, ret, "Open directory with NULL path should fail");
    
    // Clean up
    dmvfs_deinit();
}

// Test readdir with NULL pointer
void test_readdir_null_pointer(void)
{
    bool result = dmvfs_init(5, 10);
    TEST_ASSERT(result == true, "Initialize DMVFS");
    
    dmfsi_dir_entry_t entry;
    int ret = dmvfs_readdir(NULL, &entry);
    TEST_ASSERT_EQUAL(-1, ret, "Read directory with NULL pointer should fail");
    
    // Clean up
    dmvfs_deinit();
}

// Test readdir with NULL entry
void test_readdir_null_entry(void)
{
    bool result = dmvfs_init(5, 10);
    TEST_ASSERT(result == true, "Initialize DMVFS");
    
    void* dp = (void*)0x1234; // Dummy pointer
    int ret = dmvfs_readdir(dp, NULL);
    TEST_ASSERT_EQUAL(-1, ret, "Read directory with NULL entry should fail");
    
    // Clean up
    dmvfs_deinit();
}

// Test closedir with NULL pointer
void test_closedir_null_pointer(void)
{
    bool result = dmvfs_init(5, 10);
    TEST_ASSERT(result == true, "Initialize DMVFS");
    
    int ret = dmvfs_closedir(NULL);
    TEST_ASSERT_EQUAL(-1, ret, "Close directory with NULL pointer should fail");
    
    // Clean up
    dmvfs_deinit();
}

// Test closedir before init
void test_closedir_before_init(void)
{
    void* dp = (void*)0x1234; // Dummy pointer
    int ret = dmvfs_closedir(dp);
    TEST_ASSERT_EQUAL(-1, ret, "Close directory before init should fail");
}

// Test direxists before init
void test_direxists_before_init(void)
{
    int ret = dmvfs_direxists("/");
    TEST_ASSERT_EQUAL(-1, ret, "Check directory existence before init should fail");
}

// Test direxists with NULL path
void test_direxists_null_path(void)
{
    bool result = dmvfs_init(5, 10);
    TEST_ASSERT(result == true, "Initialize DMVFS");
    
    int ret = dmvfs_direxists(NULL);
    TEST_ASSERT_EQUAL(-1, ret, "Check directory existence with NULL path should fail");
    
    // Clean up
    dmvfs_deinit();
}

int main(void)
{
    printf("========================================\n");
    printf("DMVFS Directory Operations Tests\n");
    printf("========================================\n");
    
    RUN_TEST(test_mkdir_before_init);
    RUN_TEST(test_mkdir_null_path);
    RUN_TEST(test_rmdir_before_init);
    RUN_TEST(test_rmdir_null_path);
    RUN_TEST(test_chdir_before_init);
    RUN_TEST(test_chdir_null_path);
    RUN_TEST(test_opendir_before_init);
    RUN_TEST(test_opendir_null_pointer);
    RUN_TEST(test_opendir_null_path);
    RUN_TEST(test_readdir_null_pointer);
    RUN_TEST(test_readdir_null_entry);
    RUN_TEST(test_closedir_null_pointer);
    RUN_TEST(test_closedir_before_init);
    RUN_TEST(test_direxists_before_init);
    RUN_TEST(test_direxists_null_path);
    
    TEST_SUMMARY();
    return TEST_RETURN_CODE();
}
