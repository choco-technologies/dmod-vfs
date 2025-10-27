#include "test_framework.h"
#include "dmvfs.h"
#include <string.h>

// Test fopen before init
void test_fopen_before_init(void)
{
    void* fp = NULL;
    int ret = dmvfs_fopen(&fp, "/test.txt", 0, 0, 1);
    TEST_ASSERT_EQUAL(-1, ret, "Open file before init should fail");
    TEST_ASSERT_NULL(fp, "File pointer should be NULL");
}

// Test fopen with NULL pointer
void test_fopen_null_pointer(void)
{
    bool result = dmvfs_init(5, 10);
    TEST_ASSERT(result == true, "Initialize DMVFS");
    
    int ret = dmvfs_fopen(NULL, "/test.txt", 0, 0, 1);
    TEST_ASSERT_EQUAL(-1, ret, "Open file with NULL pointer should fail");
    
    // Clean up
    dmvfs_deinit();
}

// Test fopen with NULL path
void test_fopen_null_path(void)
{
    bool result = dmvfs_init(5, 10);
    TEST_ASSERT(result == true, "Initialize DMVFS");
    
    void* fp = NULL;
    int ret = dmvfs_fopen(&fp, NULL, 0, 0, 1);
    TEST_ASSERT_EQUAL(-1, ret, "Open file with NULL path should fail");
    
    // Clean up
    dmvfs_deinit();
}

// Test fclose with NULL pointer
void test_fclose_null_pointer(void)
{
    bool result = dmvfs_init(5, 10);
    TEST_ASSERT(result == true, "Initialize DMVFS");
    
    int ret = dmvfs_fclose(NULL);
    TEST_ASSERT_EQUAL(-1, ret, "Close file with NULL pointer should fail");
    
    // Clean up
    dmvfs_deinit();
}

// Test fclose before init
void test_fclose_before_init(void)
{
    void* fp = (void*)0x1234; // Dummy pointer
    int ret = dmvfs_fclose(fp);
    TEST_ASSERT_EQUAL(-1, ret, "Close file before init should fail");
}

// Test fclose_process
void test_fclose_process(void)
{
    bool result = dmvfs_init(5, 10);
    TEST_ASSERT(result == true, "Initialize DMVFS");
    
    // Close files for a non-existent process (should succeed but do nothing)
    int ret = dmvfs_fclose_process(999);
    TEST_ASSERT_EQUAL(0, ret, "Close files for non-existent process");
    
    // Clean up
    dmvfs_deinit();
}

// Test fclose_process before init
void test_fclose_process_before_init(void)
{
    int ret = dmvfs_fclose_process(1);
    TEST_ASSERT_EQUAL(-1, ret, "Close process files before init should fail");
}

// Test fread with NULL pointer
void test_fread_null_pointer(void)
{
    bool result = dmvfs_init(5, 10);
    TEST_ASSERT(result == true, "Initialize DMVFS");
    
    char buffer[100];
    size_t read_bytes = 0;
    int ret = dmvfs_fread(NULL, buffer, sizeof(buffer), &read_bytes);
    TEST_ASSERT_EQUAL(-1, ret, "Read from NULL file pointer should fail");
    
    // Clean up
    dmvfs_deinit();
}

// Test fread with NULL buffer
void test_fread_null_buffer(void)
{
    bool result = dmvfs_init(5, 10);
    TEST_ASSERT(result == true, "Initialize DMVFS");
    
    void* fp = (void*)0x1234; // Dummy pointer
    size_t read_bytes = 0;
    int ret = dmvfs_fread(fp, NULL, 100, &read_bytes);
    TEST_ASSERT_EQUAL(-1, ret, "Read with NULL buffer should fail");
    
    // Clean up
    dmvfs_deinit();
}

// Test fread with zero size
void test_fread_zero_size(void)
{
    bool result = dmvfs_init(5, 10);
    TEST_ASSERT(result == true, "Initialize DMVFS");
    
    void* fp = (void*)0x1234; // Dummy pointer
    char buffer[100];
    size_t read_bytes = 0;
    int ret = dmvfs_fread(fp, buffer, 0, &read_bytes);
    TEST_ASSERT_EQUAL(-1, ret, "Read with zero size should fail");
    
    // Clean up
    dmvfs_deinit();
}

// Test fwrite with NULL pointer
void test_fwrite_null_pointer(void)
{
    bool result = dmvfs_init(5, 10);
    TEST_ASSERT(result == true, "Initialize DMVFS");
    
    const char* data = "test";
    size_t written_bytes = 0;
    int ret = dmvfs_fwrite(NULL, data, strlen(data), &written_bytes);
    TEST_ASSERT_EQUAL(-1, ret, "Write to NULL file pointer should fail");
    
    // Clean up
    dmvfs_deinit();
}

// Test fwrite with NULL buffer
void test_fwrite_null_buffer(void)
{
    bool result = dmvfs_init(5, 10);
    TEST_ASSERT(result == true, "Initialize DMVFS");
    
    void* fp = (void*)0x1234; // Dummy pointer
    size_t written_bytes = 0;
    int ret = dmvfs_fwrite(fp, NULL, 100, &written_bytes);
    TEST_ASSERT_EQUAL(-1, ret, "Write with NULL buffer should fail");
    
    // Clean up
    dmvfs_deinit();
}

// Test lseek with NULL pointer
void test_lseek_null_pointer(void)
{
    bool result = dmvfs_init(5, 10);
    TEST_ASSERT(result == true, "Initialize DMVFS");
    
    int ret = dmvfs_lseek(NULL, 0, 0);
    TEST_ASSERT_EQUAL(-1, ret, "Seek on NULL file pointer should fail");
    
    // Clean up
    dmvfs_deinit();
}

// Test ftell with NULL pointer
void test_ftell_null_pointer(void)
{
    bool result = dmvfs_init(5, 10);
    TEST_ASSERT(result == true, "Initialize DMVFS");
    
    long ret = dmvfs_ftell(NULL);
    TEST_ASSERT_EQUAL(-1, ret, "Tell on NULL file pointer should fail");
    
    // Clean up
    dmvfs_deinit();
}

// Test feof with NULL pointer
void test_feof_null_pointer(void)
{
    bool result = dmvfs_init(5, 10);
    TEST_ASSERT(result == true, "Initialize DMVFS");
    
    int ret = dmvfs_feof(NULL);
    TEST_ASSERT_EQUAL(-1, ret, "EOF check on NULL file pointer should fail");
    
    // Clean up
    dmvfs_deinit();
}

int main(void)
{
    printf("========================================\n");
    printf("DMVFS File Operations Tests\n");
    printf("========================================\n");
    
    RUN_TEST(test_fopen_before_init);
    RUN_TEST(test_fopen_null_pointer);
    RUN_TEST(test_fopen_null_path);
    RUN_TEST(test_fclose_null_pointer);
    RUN_TEST(test_fclose_before_init);
    RUN_TEST(test_fclose_process);
    RUN_TEST(test_fclose_process_before_init);
    RUN_TEST(test_fread_null_pointer);
    RUN_TEST(test_fread_null_buffer);
    RUN_TEST(test_fread_zero_size);
    RUN_TEST(test_fwrite_null_pointer);
    RUN_TEST(test_fwrite_null_buffer);
    RUN_TEST(test_lseek_null_pointer);
    RUN_TEST(test_ftell_null_pointer);
    RUN_TEST(test_feof_null_pointer);
    
    TEST_SUMMARY();
    return TEST_RETURN_CODE();
}
