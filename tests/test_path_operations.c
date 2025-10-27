#include "test_framework.h"
#include "dmvfs.h"
#include <string.h>

// Test current working directory operations
static void test_cwd_operations(void)
{
    printf("\nTest: Current working directory operations\n");
    
    char buffer[256];
    int result;
    
    // Get current working directory (should be / initially)
    result = dmvfs_getcwd(buffer, sizeof(buffer));
    TEST_ASSERT_EQ(result, DMFSI_OK, "getcwd successful");
    printf("    Current directory: %s\n", buffer);
    
    // Create a directory to change into
    result = dmvfs_mkdir("/mnt/test_cwd", 0);
    TEST_ASSERT_EQ(result, DMFSI_OK, "Test directory created");
    
    // Change to the new directory
    result = dmvfs_chdir("/mnt/test_cwd");
    TEST_ASSERT_EQ(result, DMFSI_OK, "Changed to /mnt/test_cwd");
    
    // Verify current directory changed
    result = dmvfs_getcwd(buffer, sizeof(buffer));
    TEST_ASSERT_EQ(result, DMFSI_OK, "getcwd successful after chdir");
    TEST_ASSERT_STR_EQ(buffer, "/mnt/test_cwd", "Current directory is /mnt/test_cwd");
    
    // Change back to /mnt
    result = dmvfs_chdir("/mnt");
    TEST_ASSERT_EQ(result, DMFSI_OK, "Changed back to /mnt");
    
    result = dmvfs_getcwd(buffer, sizeof(buffer));
    TEST_ASSERT_EQ(result, DMFSI_OK, "getcwd successful");
    TEST_ASSERT_STR_EQ(buffer, "/mnt", "Current directory is /mnt");
    
    // Clean up
    result = dmvfs_rmdir("/mnt/test_cwd");
    TEST_ASSERT_EQ(result, DMFSI_OK, "Test directory removed");
}

// Test path operations with relative paths
static void test_relative_paths(void)
{
    printf("\nTest: Relative path operations\n");
    
    void* fp = NULL;
    int result;
    char abs_path[256];
    
    // Change to /mnt
    result = dmvfs_chdir("/mnt");
    TEST_ASSERT_EQ(result, DMFSI_OK, "Changed to /mnt");
    
    // Create a file using relative path
    result = dmvfs_fopen(&fp, "relative_file.txt", DMFSI_O_WRONLY | DMFSI_O_CREAT, 0, 0);
    TEST_ASSERT_EQ(result, DMFSI_OK, "File created with relative path");
    dmvfs_fclose(fp);
    
    // Verify file exists using absolute path
    dmfsi_stat_t stat;
    result = dmvfs_stat("/mnt/relative_file.txt", &stat);
    TEST_ASSERT_EQ(result, DMFSI_OK, "File exists at /mnt/relative_file.txt");
    
    // Test toabs function
    result = dmvfs_toabs("relative_file.txt", abs_path, sizeof(abs_path));
    TEST_ASSERT_EQ(result, DMFSI_OK, "toabs successful");
    TEST_ASSERT_STR_EQ(abs_path, "/mnt/relative_file.txt", "Absolute path is correct");
    
    // Clean up
    result = dmvfs_remove("/mnt/relative_file.txt");
    TEST_ASSERT_EQ(result, DMFSI_OK, "File removed");
}

// Test absolute path conversion
static void test_toabs(void)
{
    printf("\nTest: Absolute path conversion\n");
    
    char abs_path[256];
    int result;
    
    // Change to /mnt
    result = dmvfs_chdir("/mnt");
    TEST_ASSERT_EQ(result, DMFSI_OK, "Changed to /mnt");
    
    // Test converting relative path
    result = dmvfs_toabs("test.txt", abs_path, sizeof(abs_path));
    TEST_ASSERT_EQ(result, DMFSI_OK, "toabs on relative path successful");
    TEST_ASSERT_STR_EQ(abs_path, "/mnt/test.txt", "Relative path converted correctly");
    
    // Test converting already absolute path
    result = dmvfs_toabs("/absolute/path.txt", abs_path, sizeof(abs_path));
    TEST_ASSERT_EQ(result, DMFSI_OK, "toabs on absolute path successful");
    TEST_ASSERT_STR_EQ(abs_path, "/absolute/path.txt", "Absolute path unchanged");
    
    // Test with subdirectory in relative path
    result = dmvfs_toabs("subdir/file.txt", abs_path, sizeof(abs_path));
    TEST_ASSERT_EQ(result, DMFSI_OK, "toabs on relative path with subdir successful");
    TEST_ASSERT_STR_EQ(abs_path, "/mnt/subdir/file.txt", "Relative path with subdir converted correctly");
}

// Test pwd (present working directory) operations
static void test_pwd_operations(void)
{
    printf("\nTest: Present working directory operations\n");
    
    char buffer[256];
    int result;
    
    // Get present working directory
    result = dmvfs_getpwd(buffer, sizeof(buffer));
    TEST_ASSERT_EQ(result, DMFSI_OK, "getpwd successful");
    printf("    Present working directory: %s\n", buffer);
    
    // Note: pwd and cwd behavior may vary depending on implementation
    // This test just verifies the function works
}

// Test path operations with nested directories
static void test_nested_path_operations(void)
{
    printf("\nTest: Nested path operations\n");
    
    int result;
    void* fp = NULL;
    
    // Create nested directory structure
    result = dmvfs_mkdir("/mnt/path_test", 0);
    TEST_ASSERT_EQ(result, DMFSI_OK, "Root test directory created");
    
    result = dmvfs_mkdir("/mnt/path_test/level1", 0);
    TEST_ASSERT_EQ(result, DMFSI_OK, "Level 1 directory created");
    
    result = dmvfs_mkdir("/mnt/path_test/level1/level2", 0);
    TEST_ASSERT_EQ(result, DMFSI_OK, "Level 2 directory created");
    
    // Change to nested directory
    result = dmvfs_chdir("/mnt/path_test/level1/level2");
    TEST_ASSERT_EQ(result, DMFSI_OK, "Changed to nested directory");
    
    // Create file in current directory using relative path
    result = dmvfs_fopen(&fp, "nested_test.txt", DMFSI_O_WRONLY | DMFSI_O_CREAT, 0, 0);
    TEST_ASSERT_EQ(result, DMFSI_OK, "File created in nested directory");
    dmvfs_fclose(fp);
    
    // Verify file exists using absolute path
    dmfsi_stat_t stat;
    result = dmvfs_stat("/mnt/path_test/level1/level2/nested_test.txt", &stat);
    TEST_ASSERT_EQ(result, DMFSI_OK, "File exists in nested directory");
    
    // Change back to root
    result = dmvfs_chdir("/mnt");
    TEST_ASSERT_EQ(result, DMFSI_OK, "Changed back to /mnt");
    
    // Clean up
    result = dmvfs_remove("/mnt/path_test/level1/level2/nested_test.txt");
    TEST_ASSERT_EQ(result, DMFSI_OK, "File removed");
    
    result = dmvfs_rmdir("/mnt/path_test/level1/level2");
    TEST_ASSERT_EQ(result, DMFSI_OK, "Level 2 directory removed");
    
    result = dmvfs_rmdir("/mnt/path_test/level1");
    TEST_ASSERT_EQ(result, DMFSI_OK, "Level 1 directory removed");
    
    result = dmvfs_rmdir("/mnt/path_test");
    TEST_ASSERT_EQ(result, DMFSI_OK, "Root test directory removed");
}

// Test array - NULL terminated
test_case_t path_operation_tests[] = {
    {"Current Working Directory", test_cwd_operations},
    {"Relative Paths", test_relative_paths},
    {"Path to Absolute", test_toabs},
    {"Present Working Directory", test_pwd_operations},
    {"Nested Path Operations", test_nested_path_operations},
    {NULL, NULL}  // Sentinel
};
