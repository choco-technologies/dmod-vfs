#include "test_framework.h"
#include "dmvfs.h"
#include "dmod.h"
#include <string.h>

// Test initialization and deinitialization
void test_dmvfs_init_deinit(void)
{
    bool result = dmvfs_init(10, 20);
    TEST_ASSERT(result == true, "Initialize DMVFS with valid parameters");
    
    int max_mount_points = dmvfs_get_max_mount_points();
    TEST_ASSERT_EQUAL(10, max_mount_points, "Get max mount points");
    
    int max_open_files = dmvfs_get_max_open_files();
    TEST_ASSERT_EQUAL(20, max_open_files, "Get max open files");
    
    result = dmvfs_deinit();
    TEST_ASSERT(result == true, "Deinitialize DMVFS");
}

// Test mounting ramfs
void test_mount_ramfs(void)
{
    bool result = dmvfs_init(10, 20);
    TEST_ASSERT(result == true, "Initialize DMVFS");
    
    // Mount ramfs at /mnt
    result = dmvfs_mount_fs("ramfs", "/mnt", NULL);
    TEST_ASSERT(result == true, "Mount ramfs at /mnt");
    
    // Unmount
    result = dmvfs_unmount_fs("/mnt");
    TEST_ASSERT(result == true, "Unmount ramfs from /mnt");
    
    dmvfs_deinit();
}

// Test file creation and writing
void test_file_create_write(void)
{
    bool result = dmvfs_init(10, 20);
    TEST_ASSERT(result == true, "Initialize DMVFS");
    
    result = dmvfs_mount_fs("ramfs", "/mnt", NULL);
    TEST_ASSERT(result == true, "Mount ramfs");
    
    // Create and write to a file
    void* fp = NULL;
    int ret = dmvfs_fopen(&fp, "/mnt/test.txt", 0x103, 0, 1); // O_RDWR | O_CREAT
    TEST_ASSERT_EQUAL(0, ret, "Create file /mnt/test.txt");
    TEST_ASSERT_NOT_NULL(fp, "File pointer should not be NULL");
    
    const char* data = "Hello, DMVFS!";
    size_t written = 0;
    ret = dmvfs_fwrite(fp, data, strlen(data), &written);
    TEST_ASSERT_EQUAL(0, ret, "Write to file");
    TEST_ASSERT_EQUAL(strlen(data), written, "Verify bytes written");
    
    ret = dmvfs_fclose(fp);
    TEST_ASSERT_EQUAL(0, ret, "Close file");
    
    dmvfs_unmount_fs("/mnt");
    dmvfs_deinit();
}

// Test file read after write
void test_file_read(void)
{
    bool result = dmvfs_init(10, 20);
    TEST_ASSERT(result == true, "Initialize DMVFS");
    
    result = dmvfs_mount_fs("ramfs", "/mnt", NULL);
    TEST_ASSERT(result == true, "Mount ramfs");
    
    // Create and write
    void* fp = NULL;
    const char* write_data = "Test data for reading";
    dmvfs_fopen(&fp, "/mnt/read_test.txt", 0x103, 0, 1); // O_RDWR | O_CREAT
    size_t written = 0;
    dmvfs_fwrite(fp, write_data, strlen(write_data), &written);
    dmvfs_fclose(fp);
    
    // Read back
    fp = NULL;
    int ret = dmvfs_fopen(&fp, "/mnt/read_test.txt", 0x01, 0, 1); // O_RDONLY
    TEST_ASSERT_EQUAL(0, ret, "Open file for reading");
    
    char buffer[100] = {0};
    size_t read_bytes = 0;
    ret = dmvfs_fread(fp, buffer, sizeof(buffer), &read_bytes);
    TEST_ASSERT_EQUAL(0, ret, "Read from file");
    TEST_ASSERT_EQUAL(strlen(write_data), read_bytes, "Verify bytes read");
    TEST_ASSERT_STR_EQUAL(write_data, buffer, "Verify file content");
    
    ret = dmvfs_fclose(fp);
    TEST_ASSERT_EQUAL(0, ret, "Close file");
    
    dmvfs_unmount_fs("/mnt");
    dmvfs_deinit();
}

// Test file seek and tell
void test_file_seek_tell(void)
{
    bool result = dmvfs_init(10, 20);
    TEST_ASSERT(result == true, "Initialize DMVFS");
    
    result = dmvfs_mount_fs("ramfs", "/mnt", NULL);
    TEST_ASSERT(result == true, "Mount ramfs");
    
    // Create file with known content
    void* fp = NULL;
    const char* data = "0123456789";
    dmvfs_fopen(&fp, "/mnt/seek_test.txt", 0x103, 0, 1);
    size_t written = 0;
    dmvfs_fwrite(fp, data, strlen(data), &written);
    dmvfs_fclose(fp);
    
    // Open and test seek/tell
    fp = NULL;
    dmvfs_fopen(&fp, "/mnt/seek_test.txt", 0x01, 0, 1);
    
    // Test initial position
    long pos = dmvfs_ftell(fp);
    TEST_ASSERT_EQUAL(0, pos, "Initial position should be 0");
    
    // Seek to middle
    int ret = dmvfs_lseek(fp, 5, 0); // SEEK_SET
    TEST_ASSERT(ret >= 0, "Seek to position 5");
    
    pos = dmvfs_ftell(fp);
    TEST_ASSERT_EQUAL(5, pos, "Position should be 5");
    
    // Read from position 5
    char buffer[10] = {0};
    size_t read_bytes = 0;
    dmvfs_fread(fp, buffer, 5, &read_bytes);
    TEST_ASSERT_STR_EQUAL("56789", buffer, "Read from position 5");
    
    dmvfs_fclose(fp);
    dmvfs_unmount_fs("/mnt");
    dmvfs_deinit();
}

// Test directory operations
void test_directory_operations(void)
{
    bool result = dmvfs_init(10, 20);
    TEST_ASSERT(result == true, "Initialize DMVFS");
    
    result = dmvfs_mount_fs("ramfs", "/mnt", NULL);
    TEST_ASSERT(result == true, "Mount ramfs");
    
    // Create directory
    int ret = dmvfs_mkdir("/mnt/testdir", 0755);
    TEST_ASSERT_EQUAL(0, ret, "Create directory /mnt/testdir");
    
    // Check if directory exists
    ret = dmvfs_direxists("/mnt/testdir");
    TEST_ASSERT(ret == 1, "Directory should exist");
    
    // Remove directory
    ret = dmvfs_rmdir("/mnt/testdir");
    TEST_ASSERT_EQUAL(0, ret, "Remove directory");
    
    // Check if directory no longer exists
    ret = dmvfs_direxists("/mnt/testdir");
    TEST_ASSERT(ret == 0, "Directory should not exist");
    
    dmvfs_unmount_fs("/mnt");
    dmvfs_deinit();
}

// Test path operations
void test_path_operations(void)
{
    bool result = dmvfs_init(10, 20);
    TEST_ASSERT(result == true, "Initialize DMVFS");
    
    // Test getcwd
    char buffer[256];
    int ret = dmvfs_getcwd(buffer, sizeof(buffer));
    TEST_ASSERT_EQUAL(0, ret, "Get current working directory");
    TEST_ASSERT_STR_EQUAL("/", buffer, "Default CWD should be /");
    
    // Test toabs with relative path
    char abs_path[256];
    ret = dmvfs_toabs("test/path", abs_path, sizeof(abs_path));
    TEST_ASSERT_EQUAL(0, ret, "Convert relative path to absolute");
    TEST_ASSERT(abs_path[0] == '/', "Absolute path should start with /");
    
    dmvfs_deinit();
}

// Test multiple file operations
void test_multiple_files(void)
{
    bool result = dmvfs_init(10, 20);
    TEST_ASSERT(result == true, "Initialize DMVFS");
    
    result = dmvfs_mount_fs("ramfs", "/mnt", NULL);
    TEST_ASSERT(result == true, "Mount ramfs");
    
    // Create multiple files
    void* fp1 = NULL;
    void* fp2 = NULL;
    
    int ret = dmvfs_fopen(&fp1, "/mnt/file1.txt", 0x103, 0, 1);
    TEST_ASSERT_EQUAL(0, ret, "Create file1.txt");
    
    ret = dmvfs_fopen(&fp2, "/mnt/file2.txt", 0x103, 0, 2);
    TEST_ASSERT_EQUAL(0, ret, "Create file2.txt");
    
    // Write to both
    const char* data1 = "File 1 content";
    const char* data2 = "File 2 content";
    size_t written = 0;
    
    dmvfs_fwrite(fp1, data1, strlen(data1), &written);
    TEST_ASSERT_EQUAL(strlen(data1), written, "Write to file1");
    
    dmvfs_fwrite(fp2, data2, strlen(data2), &written);
    TEST_ASSERT_EQUAL(strlen(data2), written, "Write to file2");
    
    // Close both
    dmvfs_fclose(fp1);
    dmvfs_fclose(fp2);
    
    // Verify file1 content
    fp1 = NULL;
    dmvfs_fopen(&fp1, "/mnt/file1.txt", 0x01, 0, 1);
    char buffer[50] = {0};
    size_t read_bytes = 0;
    dmvfs_fread(fp1, buffer, sizeof(buffer), &read_bytes);
    TEST_ASSERT_STR_EQUAL(data1, buffer, "Verify file1 content");
    dmvfs_fclose(fp1);
    
    dmvfs_unmount_fs("/mnt");
    dmvfs_deinit();
}

// Test file removal
void test_file_remove(void)
{
    bool result = dmvfs_init(10, 20);
    TEST_ASSERT(result == true, "Initialize DMVFS");
    
    result = dmvfs_mount_fs("ramfs", "/mnt", NULL);
    TEST_ASSERT(result == true, "Mount ramfs");
    
    // Create a file
    void* fp = NULL;
    dmvfs_fopen(&fp, "/mnt/remove_test.txt", 0x103, 0, 1);
    dmvfs_fwrite(fp, "test", 4, NULL);
    dmvfs_fclose(fp);
    
    // Remove the file
    int ret = dmvfs_unlink("/mnt/remove_test.txt");
    TEST_ASSERT_EQUAL(0, ret, "Remove file");
    
    // Try to open removed file (should fail)
    fp = NULL;
    ret = dmvfs_fopen(&fp, "/mnt/remove_test.txt", 0x01, 0, 1);
    TEST_ASSERT(ret != 0, "Opening removed file should fail");
    
    dmvfs_unmount_fs("/mnt");
    dmvfs_deinit();
}

int main(void)
{
    printf("========================================\n");
    printf("DMVFS Integration Tests with RamFS\n");
    printf("========================================\n");
    
    // In SYSTEM mode, modules are statically linked
    // Call dmod_init for ramfs to ensure it's initialized
    extern int dmod_init(const Dmod_Config_t* config);
    Dmod_Config_t config = {0};
    if (dmod_init(&config) != 0) {
        printf("Warning: RamFS initialization returned non-zero\n");
    }
    
    RUN_TEST(test_dmvfs_init_deinit);
    RUN_TEST(test_mount_ramfs);
    RUN_TEST(test_file_create_write);
    RUN_TEST(test_file_read);
    RUN_TEST(test_file_seek_tell);
    RUN_TEST(test_directory_operations);
    RUN_TEST(test_path_operations);
    RUN_TEST(test_multiple_files);
    RUN_TEST(test_file_remove);
    
    TEST_SUMMARY();
    
    // Cleanup
    extern int dmod_deinit(void);
    dmod_deinit();
    
    return TEST_RETURN_CODE();
}
