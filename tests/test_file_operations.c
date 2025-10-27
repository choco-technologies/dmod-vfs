#include "test_framework.h"
#include "dmvfs.h"
#include <string.h>

// Test file creation and basic operations
static void test_file_create_and_write(void)
{
    printf("\nTest: File creation and write\n");
    
    void* fp = NULL;
    int result;
    size_t written;
    const char* test_data = "Hello, DMVFS!";
    size_t test_data_len = strlen(test_data);
    
    // Create and open file for writing
    result = dmvfs_fopen(&fp, "/mnt/test_file.txt", DMFSI_O_WRONLY | DMFSI_O_CREAT, 0, 0);
    TEST_ASSERT_EQ(result, DMFSI_OK, "File opened successfully for writing");
    TEST_ASSERT(fp != NULL, "File pointer is not NULL");
    
    // Write data to file
    result = dmvfs_fwrite(fp, test_data, test_data_len, &written);
    TEST_ASSERT_EQ(result, DMFSI_OK, "Data written successfully");
    TEST_ASSERT_EQ(written, test_data_len, "All bytes were written");
    
    // Close file
    result = dmvfs_fclose(fp);
    TEST_ASSERT_EQ(result, DMFSI_OK, "File closed successfully");
}

// Test file reading
static void test_file_read(void)
{
    printf("\nTest: File read\n");
    
    void* fp = NULL;
    int result;
    size_t read_bytes;
    char buffer[100] = {0};
    const char* expected_data = "Hello, DMVFS!";
    
    // Open file for reading
    result = dmvfs_fopen(&fp, "/mnt/test_file.txt", DMFSI_O_RDONLY, 0, 0);
    TEST_ASSERT_EQ(result, DMFSI_OK, "File opened successfully for reading");
    TEST_ASSERT(fp != NULL, "File pointer is not NULL");
    
    // Read data from file
    result = dmvfs_fread(fp, buffer, sizeof(buffer) - 1, &read_bytes);
    TEST_ASSERT_EQ(result, DMFSI_OK, "Data read successfully");
    TEST_ASSERT_EQ(read_bytes, strlen(expected_data), "Expected number of bytes read");
    TEST_ASSERT_STR_EQ(buffer, expected_data, "Read data matches written data");
    
    // Close file
    result = dmvfs_fclose(fp);
    TEST_ASSERT_EQ(result, DMFSI_OK, "File closed successfully");
}

// Test file seek operations
static void test_file_seek(void)
{
    printf("\nTest: File seek operations\n");
    
    void* fp = NULL;
    int result;
    long pos;
    char buffer[10] = {0};
    size_t read_bytes;
    
    // Open file for reading
    result = dmvfs_fopen(&fp, "/mnt/test_file.txt", DMFSI_O_RDONLY, 0, 0);
    TEST_ASSERT_EQ(result, DMFSI_OK, "File opened successfully");
    
    // Test SEEK_SET
    result = dmvfs_lseek(fp, 7, DMFSI_SEEK_SET);
    TEST_ASSERT_EQ(result, DMFSI_OK, "Seek to position 7 successful");
    
    pos = dmvfs_ftell(fp);
    TEST_ASSERT_EQ(pos, 7, "File position is 7");
    
    // Read from position 7
    result = dmvfs_fread(fp, buffer, 5, &read_bytes);
    TEST_ASSERT_EQ(result, DMFSI_OK, "Read from position 7 successful");
    TEST_ASSERT_STR_EQ(buffer, "DMVFS", "Read correct data from position 7");
    
    // Test SEEK_CUR
    result = dmvfs_lseek(fp, -5, DMFSI_SEEK_CUR);
    TEST_ASSERT_EQ(result, DMFSI_OK, "Seek backward from current position successful");
    
    pos = dmvfs_ftell(fp);
    TEST_ASSERT_EQ(pos, 7, "File position is back to 7");
    
    // Test SEEK_END
    result = dmvfs_lseek(fp, 0, DMFSI_SEEK_END);
    TEST_ASSERT_EQ(result, DMFSI_OK, "Seek to end successful");
    
    pos = dmvfs_ftell(fp);
    TEST_ASSERT_EQ(pos, 13, "File position is at end (13)");
    
    // Test EOF
    result = dmvfs_feof(fp);
    TEST_ASSERT(result != 0, "EOF flag is set");
    
    // Close file
    result = dmvfs_fclose(fp);
    TEST_ASSERT_EQ(result, DMFSI_OK, "File closed successfully");
}

// Test file append operations
static void test_file_append(void)
{
    printf("\nTest: File append operations\n");
    
    void* fp = NULL;
    int result;
    size_t written;
    const char* append_data = " Appended!";
    
    // Open file for appending
    result = dmvfs_fopen(&fp, "/mnt/test_file.txt", DMFSI_O_WRONLY | DMFSI_O_APPEND, 0, 0);
    TEST_ASSERT_EQ(result, DMFSI_OK, "File opened successfully for appending");
    
    // Write data
    result = dmvfs_fwrite(fp, append_data, strlen(append_data), &written);
    TEST_ASSERT_EQ(result, DMFSI_OK, "Data appended successfully");
    TEST_ASSERT_EQ(written, strlen(append_data), "All bytes were written");
    
    // Close file
    result = dmvfs_fclose(fp);
    TEST_ASSERT_EQ(result, DMFSI_OK, "File closed successfully");
    
    // Verify appended data
    char buffer[100] = {0};
    size_t read_bytes;
    
    result = dmvfs_fopen(&fp, "/mnt/test_file.txt", DMFSI_O_RDONLY, 0, 0);
    TEST_ASSERT_EQ(result, DMFSI_OK, "File reopened for reading");
    
    result = dmvfs_fread(fp, buffer, sizeof(buffer) - 1, &read_bytes);
    TEST_ASSERT_EQ(result, DMFSI_OK, "Data read successfully");
    TEST_ASSERT_STR_EQ(buffer, "Hello, DMVFS! Appended!", "Appended data is correct");
    
    result = dmvfs_fclose(fp);
    TEST_ASSERT_EQ(result, DMFSI_OK, "File closed successfully");
}

// Test file truncate operations
static void test_file_truncate(void)
{
    printf("\nTest: File truncate operations\n");
    
    void* fp = NULL;
    int result;
    size_t written;
    const char* new_data = "New content";
    
    // Open file with truncate flag
    result = dmvfs_fopen(&fp, "/mnt/test_file.txt", DMFSI_O_WRONLY | DMFSI_O_TRUNC, 0, 0);
    TEST_ASSERT_EQ(result, DMFSI_OK, "File opened with truncate flag");
    
    // Write new data
    result = dmvfs_fwrite(fp, new_data, strlen(new_data), &written);
    TEST_ASSERT_EQ(result, DMFSI_OK, "New data written successfully");
    
    // Close file
    result = dmvfs_fclose(fp);
    TEST_ASSERT_EQ(result, DMFSI_OK, "File closed successfully");
    
    // Verify file contains only new data
    char buffer[100] = {0};
    size_t read_bytes;
    
    result = dmvfs_fopen(&fp, "/mnt/test_file.txt", DMFSI_O_RDONLY, 0, 0);
    TEST_ASSERT_EQ(result, DMFSI_OK, "File reopened for reading");
    
    result = dmvfs_fread(fp, buffer, sizeof(buffer) - 1, &read_bytes);
    TEST_ASSERT_EQ(result, DMFSI_OK, "Data read successfully");
    TEST_ASSERT_EQ(read_bytes, strlen(new_data), "Read correct number of bytes");
    TEST_ASSERT_STR_EQ(buffer, new_data, "File contains only new data");
    
    result = dmvfs_fclose(fp);
    TEST_ASSERT_EQ(result, DMFSI_OK, "File closed successfully");
}

// Test file stat operations
static void test_file_stat(void)
{
    printf("\nTest: File stat operations\n");
    
    dmfsi_stat_t stat;
    int result;
    
    // Get file statistics
    result = dmvfs_stat("/mnt/test_file.txt", &stat);
    TEST_ASSERT_EQ(result, DMFSI_OK, "File stat retrieved successfully");
    TEST_ASSERT_EQ(stat.size, 11, "File size is correct (11 bytes)");
    TEST_ASSERT((stat.attr & DMFSI_ATTR_DIRECTORY) == 0, "File is not a directory");
}

// Test file remove operation
static void test_file_remove(void)
{
    printf("\nTest: File remove operations\n");
    
    int result;
    
    // Remove file
    result = dmvfs_remove("/mnt/test_file.txt");
    TEST_ASSERT_EQ(result, DMFSI_OK, "File removed successfully");
    
    // Try to stat removed file
    dmfsi_stat_t stat;
    result = dmvfs_stat("/mnt/test_file.txt", &stat);
    TEST_ASSERT_NEQ(result, DMFSI_OK, "Removed file does not exist");
}

// Test file rename operation
static void test_file_rename(void)
{
    printf("\nTest: File rename operations\n");
    
    void* fp = NULL;
    int result;
    size_t written;
    const char* test_data = "Rename test";
    
    // Create a file
    result = dmvfs_fopen(&fp, "/mnt/old_name.txt", DMFSI_O_WRONLY | DMFSI_O_CREAT, 0, 0);
    TEST_ASSERT_EQ(result, DMFSI_OK, "File created for rename test");
    
    result = dmvfs_fwrite(fp, test_data, strlen(test_data), &written);
    TEST_ASSERT_EQ(result, DMFSI_OK, "Data written to file");
    
    result = dmvfs_fclose(fp);
    TEST_ASSERT_EQ(result, DMFSI_OK, "File closed");
    
    // Rename file
    result = dmvfs_rename("/mnt/old_name.txt", "/mnt/new_name.txt");
    TEST_ASSERT_EQ(result, DMFSI_OK, "File renamed successfully");
    
    // Verify old name doesn't exist
    dmfsi_stat_t stat;
    result = dmvfs_stat("/mnt/old_name.txt", &stat);
    TEST_ASSERT_NEQ(result, DMFSI_OK, "Old file name does not exist");
    
    // Verify new name exists
    result = dmvfs_stat("/mnt/new_name.txt", &stat);
    TEST_ASSERT_EQ(result, DMFSI_OK, "New file name exists");
    
    // Clean up
    result = dmvfs_remove("/mnt/new_name.txt");
    TEST_ASSERT_EQ(result, DMFSI_OK, "Renamed file removed");
}

// Test character I/O operations
static void test_file_char_io(void)
{
    printf("\nTest: Character I/O operations\n");
    
    void* fp = NULL;
    int result;
    int ch;
    const char test_chars[] = {'A', 'B', 'C'};
    const int num_chars = sizeof(test_chars) / sizeof(test_chars[0]);
    
    // Create a file and write some characters
    result = dmvfs_fopen(&fp, "/mnt/char_test.txt", DMFSI_O_WRONLY | DMFSI_O_CREAT | DMFSI_O_TRUNC, 0, 0);
    TEST_ASSERT_EQ(result, DMFSI_OK, "File created for char I/O test");
    
    // Write characters using putc
    for (int i = 0; i < num_chars; i++) {
        result = dmvfs_putc(fp, test_chars[i]);
        char msg[64];
        sprintf(msg, "Character '%c' written", test_chars[i]);
        TEST_ASSERT_EQ(result, DMFSI_OK, msg);
    }
    
    result = dmvfs_fclose(fp);
    TEST_ASSERT_EQ(result, DMFSI_OK, "File closed");
    
    // Read characters using getc
    result = dmvfs_fopen(&fp, "/mnt/char_test.txt", DMFSI_O_RDONLY, 0, 0);
    TEST_ASSERT_EQ(result, DMFSI_OK, "File opened for reading");
    
    for (int i = 0; i < num_chars; i++) {
        ch = dmvfs_getc(fp);
        TEST_ASSERT_EQ(ch, test_chars[i], "Read correct character");
    }
    
    result = dmvfs_fclose(fp);
    TEST_ASSERT_EQ(result, DMFSI_OK, "File closed");
    
    // Clean up
    result = dmvfs_remove("/mnt/char_test.txt");
    TEST_ASSERT_EQ(result, DMFSI_OK, "Test file removed");
}

// Test array - NULL terminated
test_case_t file_operation_tests[] = {
    {"File Create and Write", test_file_create_and_write},
    {"File Read", test_file_read},
    {"File Seek", test_file_seek},
    {"File Append", test_file_append},
    {"File Truncate", test_file_truncate},
    {"File Stat", test_file_stat},
    {"File Remove", test_file_remove},
    {"File Rename", test_file_rename},
    {"File Character I/O", test_file_char_io},
    {NULL, NULL}  // Sentinel
};
