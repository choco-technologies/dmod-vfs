#include "test_framework.h"
#include "dmvfs.h"
#include <string.h>

// Test directory creation
static void test_dir_create(void)
{
    printf("\nTest: Directory creation\n");
    
    int result;
    
    // Create a directory
    result = dmvfs_mkdir("/mnt/test_dir", 0);
    TEST_ASSERT_EQ(result, DMFSI_OK, "Directory created successfully");
    
    // Verify directory exists
    result = dmvfs_direxists("/mnt/test_dir");
    TEST_ASSERT(result != 0, "Directory exists");
}

// Test directory stat
static void test_dir_stat(void)
{
    printf("\nTest: Directory stat\n");
    
    dmfsi_stat_t stat;
    int result;
    
    // Get directory statistics
    result = dmvfs_stat("/mnt/test_dir", &stat);
    TEST_ASSERT_EQ(result, DMFSI_OK, "Directory stat retrieved successfully");
    TEST_ASSERT((stat.attr & DMFSI_ATTR_DIRECTORY) != 0, "Entry is a directory");
}

// Test directory operations with files
static void test_dir_with_files(void)
{
    printf("\nTest: Directory operations with files\n");
    
    void* fp = NULL;
    int result;
    size_t written;
    const char* test_data = "File in directory";
    
    // Create a file in the directory
    result = dmvfs_fopen(&fp, "/mnt/test_dir/file1.txt", DMFSI_O_WRONLY | DMFSI_O_CREAT, 0, 0);
    TEST_ASSERT_EQ(result, DMFSI_OK, "File created in directory");
    
    result = dmvfs_fwrite(fp, test_data, strlen(test_data), &written);
    TEST_ASSERT_EQ(result, DMFSI_OK, "Data written to file in directory");
    
    result = dmvfs_fclose(fp);
    TEST_ASSERT_EQ(result, DMFSI_OK, "File closed");
    
    // Verify file exists
    dmfsi_stat_t stat;
    result = dmvfs_stat("/mnt/test_dir/file1.txt", &stat);
    TEST_ASSERT_EQ(result, DMFSI_OK, "File in directory exists");
    TEST_ASSERT((stat.attr & DMFSI_ATTR_DIRECTORY) == 0, "Entry is a file, not directory");
}

// Test reading directory contents
static void test_dir_read(void)
{
    printf("\nTest: Reading directory contents\n");
    
    void* dp = NULL;
    dmfsi_dir_entry_t entry;
    int result;
    int file_count = 0;
    bool found_file1 = false;
    
    // Create additional files for directory listing
    void* fp = NULL;
    result = dmvfs_fopen(&fp, "/mnt/test_dir/file2.txt", DMFSI_O_WRONLY | DMFSI_O_CREAT, 0, 0);
    TEST_ASSERT_EQ(result, DMFSI_OK, "Second file created in directory");
    dmvfs_fclose(fp);
    
    result = dmvfs_fopen(&fp, "/mnt/test_dir/file3.txt", DMFSI_O_WRONLY | DMFSI_O_CREAT, 0, 0);
    TEST_ASSERT_EQ(result, DMFSI_OK, "Third file created in directory");
    dmvfs_fclose(fp);
    
    // Open directory
    result = dmvfs_opendir(&dp, "/mnt/test_dir");
    TEST_ASSERT_EQ(result, DMFSI_OK, "Directory opened successfully");
    TEST_ASSERT(dp != NULL, "Directory pointer is not NULL");
    
    // Read directory entries
    while (dmvfs_readdir(dp, &entry) == DMFSI_OK) {
        printf("    Found entry: %s\n", entry.name);
        
        // Skip . and .. entries if present
        if (strcmp(entry.name, ".") == 0 || strcmp(entry.name, "..") == 0) {
            continue;
        }
        
        file_count++;
        if (strcmp(entry.name, "file1.txt") == 0) {
            found_file1 = true;
        }
    }
    
    TEST_ASSERT(file_count >= 3, "Found at least 3 files in directory");
    TEST_ASSERT(found_file1, "Found file1.txt in directory");
    
    // Close directory
    result = dmvfs_closedir(dp);
    TEST_ASSERT_EQ(result, DMFSI_OK, "Directory closed successfully");
}

// Test nested directories
static void test_nested_dirs(void)
{
    printf("\nTest: Nested directories\n");
    
    int result;
    
    // Create nested directory structure
    result = dmvfs_mkdir("/mnt/test_dir/subdir1", 0);
    TEST_ASSERT_EQ(result, DMFSI_OK, "Subdirectory created successfully");
    
    result = dmvfs_mkdir("/mnt/test_dir/subdir1/subdir2", 0);
    TEST_ASSERT_EQ(result, DMFSI_OK, "Nested subdirectory created successfully");
    
    // Verify nested directories exist
    result = dmvfs_direxists("/mnt/test_dir/subdir1");
    TEST_ASSERT(result != 0, "First subdirectory exists");
    
    result = dmvfs_direxists("/mnt/test_dir/subdir1/subdir2");
    TEST_ASSERT(result != 0, "Nested subdirectory exists");
    
    // Create a file in nested directory
    void* fp = NULL;
    result = dmvfs_fopen(&fp, "/mnt/test_dir/subdir1/subdir2/nested_file.txt", 
                         DMFSI_O_WRONLY | DMFSI_O_CREAT, 0, 0);
    TEST_ASSERT_EQ(result, DMFSI_OK, "File created in nested directory");
    dmvfs_fclose(fp);
    
    // Verify file exists
    dmfsi_stat_t stat;
    result = dmvfs_stat("/mnt/test_dir/subdir1/subdir2/nested_file.txt", &stat);
    TEST_ASSERT_EQ(result, DMFSI_OK, "File in nested directory exists");
}

// Test directory removal
static void test_dir_remove(void)
{
    printf("\nTest: Directory removal\n");
    
    int result;
    
    // Try to remove non-empty directory (should fail)
    result = dmvfs_rmdir("/mnt/test_dir");
    TEST_ASSERT_NEQ(result, DMFSI_OK, "Cannot remove non-empty directory");
    
    // Remove files from nested directory
    result = dmvfs_remove("/mnt/test_dir/subdir1/subdir2/nested_file.txt");
    TEST_ASSERT_EQ(result, DMFSI_OK, "File removed from nested directory");
    
    // Remove nested directories
    result = dmvfs_rmdir("/mnt/test_dir/subdir1/subdir2");
    TEST_ASSERT_EQ(result, DMFSI_OK, "Nested subdirectory removed");
    
    result = dmvfs_rmdir("/mnt/test_dir/subdir1");
    TEST_ASSERT_EQ(result, DMFSI_OK, "Subdirectory removed");
    
    // Remove files from test_dir
    result = dmvfs_remove("/mnt/test_dir/file1.txt");
    TEST_ASSERT_EQ(result, DMFSI_OK, "file1.txt removed");
    
    result = dmvfs_remove("/mnt/test_dir/file2.txt");
    TEST_ASSERT_EQ(result, DMFSI_OK, "file2.txt removed");
    
    result = dmvfs_remove("/mnt/test_dir/file3.txt");
    TEST_ASSERT_EQ(result, DMFSI_OK, "file3.txt removed");
    
    // Now remove the empty directory
    result = dmvfs_rmdir("/mnt/test_dir");
    TEST_ASSERT_EQ(result, DMFSI_OK, "Empty directory removed successfully");
    
    // Verify directory no longer exists
    result = dmvfs_direxists("/mnt/test_dir");
    TEST_ASSERT(result == 0, "Removed directory does not exist");
}

// Test array - NULL terminated
test_case_t directory_operation_tests[] = {
    {"Directory Create", test_dir_create},
    {"Directory Stat", test_dir_stat},
    {"Directory with Files", test_dir_with_files},
    {"Directory Read", test_dir_read},
    {"Nested Directories", test_nested_dirs},
    {"Directory Remove", test_dir_remove},
    {NULL, NULL}  // Sentinel
};
