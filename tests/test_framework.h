#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <stdio.h>
#include <stdbool.h>

// Test statistics
static int total_tests = 0;
static int passed_tests = 0;
static int failed_tests = 0;

// Color codes for output
#define COLOR_RED     "\033[0;31m"
#define COLOR_GREEN   "\033[0;32m"
#define COLOR_YELLOW  "\033[0;33m"
#define COLOR_RESET   "\033[0m"

// Test assertion macros
#define TEST_ASSERT(condition, message) \
    do { \
        total_tests++; \
        if (condition) { \
            passed_tests++; \
            printf(COLOR_GREEN "[PASS]" COLOR_RESET " %s\n", message); \
        } else { \
            failed_tests++; \
            printf(COLOR_RED "[FAIL]" COLOR_RESET " %s\n", message); \
            printf("       at %s:%d\n", __FILE__, __LINE__); \
        } \
    } while(0)

#define TEST_ASSERT_EQUAL(expected, actual, message) \
    do { \
        total_tests++; \
        if ((expected) == (actual)) { \
            passed_tests++; \
            printf(COLOR_GREEN "[PASS]" COLOR_RESET " %s\n", message); \
        } else { \
            failed_tests++; \
            printf(COLOR_RED "[FAIL]" COLOR_RESET " %s (expected: %d, actual: %d)\n", \
                   message, (int)(expected), (int)(actual)); \
            printf("       at %s:%d\n", __FILE__, __LINE__); \
        } \
    } while(0)

#define TEST_ASSERT_NOT_NULL(ptr, message) \
    do { \
        total_tests++; \
        if ((ptr) != NULL) { \
            passed_tests++; \
            printf(COLOR_GREEN "[PASS]" COLOR_RESET " %s\n", message); \
        } else { \
            failed_tests++; \
            printf(COLOR_RED "[FAIL]" COLOR_RESET " %s (pointer is NULL)\n", message); \
            printf("       at %s:%d\n", __FILE__, __LINE__); \
        } \
    } while(0)

#define TEST_ASSERT_NULL(ptr, message) \
    do { \
        total_tests++; \
        if ((ptr) == NULL) { \
            passed_tests++; \
            printf(COLOR_GREEN "[PASS]" COLOR_RESET " %s\n", message); \
        } else { \
            failed_tests++; \
            printf(COLOR_RED "[FAIL]" COLOR_RESET " %s (pointer is not NULL)\n", message); \
            printf("       at %s:%d\n", __FILE__, __LINE__); \
        } \
    } while(0)

#define TEST_ASSERT_STR_EQUAL(expected, actual, message) \
    do { \
        total_tests++; \
        if (strcmp((expected), (actual)) == 0) { \
            passed_tests++; \
            printf(COLOR_GREEN "[PASS]" COLOR_RESET " %s\n", message); \
        } else { \
            failed_tests++; \
            printf(COLOR_RED "[FAIL]" COLOR_RESET " %s (expected: '%s', actual: '%s')\n", \
                   message, (expected), (actual)); \
            printf("       at %s:%d\n", __FILE__, __LINE__); \
        } \
    } while(0)

// Test suite macros
#define RUN_TEST(test_func) \
    do { \
        printf("\n" COLOR_YELLOW "Running: " COLOR_RESET "%s\n", #test_func); \
        test_func(); \
    } while(0)

#define TEST_SUMMARY() \
    do { \
        printf("\n========================================\n"); \
        printf("Test Summary:\n"); \
        printf("  Total:  %d\n", total_tests); \
        printf("  " COLOR_GREEN "Passed: %d" COLOR_RESET "\n", passed_tests); \
        if (failed_tests > 0) { \
            printf("  " COLOR_RED "Failed: %d" COLOR_RESET "\n", failed_tests); \
        } else { \
            printf("  Failed: %d\n", failed_tests); \
        } \
        printf("========================================\n"); \
    } while(0)

#define TEST_RETURN_CODE() (failed_tests > 0 ? 1 : 0)

#endif // TEST_FRAMEWORK_H
