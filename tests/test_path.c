#include "test_framework.h"
#include "dmvfs.h"
#include <string.h>

// Test getcwd after initialization
void test_getcwd_after_init(void)
{
    bool result = dmvfs_init(5, 10);
    TEST_ASSERT(result == true, "Initialize DMVFS");
    
    char buffer[256];
    int ret = dmvfs_getcwd(buffer, sizeof(buffer));
    TEST_ASSERT_EQUAL(0, ret, "Get current working directory");
    TEST_ASSERT_STR_EQUAL("/", buffer, "Default CWD should be root");
    
    // Clean up
    dmvfs_deinit();
}

// Test getcwd with small buffer
void test_getcwd_small_buffer(void)
{
    bool result = dmvfs_init(5, 10);
    TEST_ASSERT(result == true, "Initialize DMVFS");
    
    char buffer[2];
    int ret = dmvfs_getcwd(buffer, sizeof(buffer));
    TEST_ASSERT_EQUAL(0, ret, "Get CWD with exact size should succeed");
    
    // Clean up
    dmvfs_deinit();
}

// Test getcwd with NULL buffer
void test_getcwd_null_buffer(void)
{
    bool result = dmvfs_init(5, 10);
    TEST_ASSERT(result == true, "Initialize DMVFS");
    
    int ret = dmvfs_getcwd(NULL, 256);
    TEST_ASSERT_EQUAL(-1, ret, "Get CWD with NULL buffer should fail");
    
    // Clean up
    dmvfs_deinit();
}

// Test getcwd before init
void test_getcwd_before_init(void)
{
    char buffer[256];
    int ret = dmvfs_getcwd(buffer, sizeof(buffer));
    TEST_ASSERT_EQUAL(-1, ret, "Get CWD before init should fail");
}

// Test getpwd after initialization
void test_getpwd_after_init(void)
{
    bool result = dmvfs_init(5, 10);
    TEST_ASSERT(result == true, "Initialize DMVFS");
    
    char buffer[256];
    int ret = dmvfs_getpwd(buffer, sizeof(buffer));
    TEST_ASSERT_EQUAL(0, ret, "Get process working directory");
    TEST_ASSERT_STR_EQUAL("/", buffer, "Default PWD should be root");
    
    // Clean up
    dmvfs_deinit();
}

// Test getpwd with NULL buffer
void test_getpwd_null_buffer(void)
{
    bool result = dmvfs_init(5, 10);
    TEST_ASSERT(result == true, "Initialize DMVFS");
    
    int ret = dmvfs_getpwd(NULL, 256);
    TEST_ASSERT_EQUAL(-1, ret, "Get PWD with NULL buffer should fail");
    
    // Clean up
    dmvfs_deinit();
}

// Test toabs with absolute path
void test_toabs_absolute_path(void)
{
    bool result = dmvfs_init(5, 10);
    TEST_ASSERT(result == true, "Initialize DMVFS");
    
    char abs_path[256];
    int ret = dmvfs_toabs("/test/path", abs_path, sizeof(abs_path));
    TEST_ASSERT_EQUAL(0, ret, "Convert absolute path");
    TEST_ASSERT_STR_EQUAL("/test/path", abs_path, "Absolute path should remain unchanged");
    
    // Clean up
    dmvfs_deinit();
}

// Test toabs with relative path
void test_toabs_relative_path(void)
{
    bool result = dmvfs_init(5, 10);
    TEST_ASSERT(result == true, "Initialize DMVFS");
    
    char abs_path[256];
    int ret = dmvfs_toabs("test/path", abs_path, sizeof(abs_path));
    TEST_ASSERT_EQUAL(0, ret, "Convert relative path");
    TEST_ASSERT_STR_EQUAL("/test/path", abs_path, "Relative path should be converted to absolute");
    
    // Clean up
    dmvfs_deinit();
}

// Test toabs with NULL path
void test_toabs_null_path(void)
{
    bool result = dmvfs_init(5, 10);
    TEST_ASSERT(result == true, "Initialize DMVFS");
    
    char abs_path[256];
    int ret = dmvfs_toabs(NULL, abs_path, sizeof(abs_path));
    TEST_ASSERT_EQUAL(-1, ret, "Convert NULL path should fail");
    
    // Clean up
    dmvfs_deinit();
}

// Test toabs with NULL output buffer
void test_toabs_null_output(void)
{
    bool result = dmvfs_init(5, 10);
    TEST_ASSERT(result == true, "Initialize DMVFS");
    
    int ret = dmvfs_toabs("/test", NULL, 256);
    TEST_ASSERT_EQUAL(-1, ret, "Convert path with NULL output should fail");
    
    // Clean up
    dmvfs_deinit();
}

// Test toabs with small buffer
void test_toabs_small_buffer(void)
{
    bool result = dmvfs_init(5, 10);
    TEST_ASSERT(result == true, "Initialize DMVFS");
    
    char abs_path[5];
    int ret = dmvfs_toabs("/test/long/path", abs_path, sizeof(abs_path));
    TEST_ASSERT_EQUAL(-1, ret, "Convert path with small buffer should fail");
    
    // Clean up
    dmvfs_deinit();
}

// Test toabs before init
void test_toabs_before_init(void)
{
    char abs_path[256];
    int ret = dmvfs_toabs("test", abs_path, sizeof(abs_path));
    TEST_ASSERT_EQUAL(-1, ret, "Convert path before init should fail");
}

int main(void)
{
    printf("========================================\n");
    printf("DMVFS Path Operations Tests\n");
    printf("========================================\n");
    
    RUN_TEST(test_getcwd_after_init);
    RUN_TEST(test_getcwd_small_buffer);
    RUN_TEST(test_getcwd_null_buffer);
    RUN_TEST(test_getcwd_before_init);
    RUN_TEST(test_getpwd_after_init);
    RUN_TEST(test_getpwd_null_buffer);
    RUN_TEST(test_toabs_absolute_path);
    RUN_TEST(test_toabs_relative_path);
    RUN_TEST(test_toabs_null_path);
    RUN_TEST(test_toabs_null_output);
    RUN_TEST(test_toabs_small_buffer);
    RUN_TEST(test_toabs_before_init);
    
    TEST_SUMMARY();
    return TEST_RETURN_CODE();
}
