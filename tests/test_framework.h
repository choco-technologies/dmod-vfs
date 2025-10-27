#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

// Test result tracking
typedef struct {
    int total;
    int passed;
    int failed;
} test_stats_t;

// Global test statistics
extern test_stats_t g_test_stats;

// Test assertion macros
#define TEST_ASSERT(condition, message) \
    do { \
        g_test_stats.total++; \
        if (condition) { \
            g_test_stats.passed++; \
            printf("  [PASS] %s\n", message); \
        } else { \
            g_test_stats.failed++; \
            printf("  [FAIL] %s\n", message); \
        } \
    } while(0)

#define TEST_ASSERT_EQ(actual, expected, message) \
    do { \
        g_test_stats.total++; \
        if ((actual) == (expected)) { \
            g_test_stats.passed++; \
            printf("  [PASS] %s\n", message); \
        } else { \
            g_test_stats.failed++; \
            printf("  [FAIL] %s (expected: %d, got: %d)\n", message, (int)(expected), (int)(actual)); \
        } \
    } while(0)

#define TEST_ASSERT_NEQ(actual, not_expected, message) \
    do { \
        g_test_stats.total++; \
        if ((actual) != (not_expected)) { \
            g_test_stats.passed++; \
            printf("  [PASS] %s\n", message); \
        } else { \
            g_test_stats.failed++; \
            printf("  [FAIL] %s (should not be: %d)\n", message, (int)(not_expected)); \
        } \
    } while(0)

#define TEST_ASSERT_STR_EQ(actual, expected, message) \
    do { \
        g_test_stats.total++; \
        if (strcmp((actual), (expected)) == 0) { \
            g_test_stats.passed++; \
            printf("  [PASS] %s\n", message); \
        } else { \
            g_test_stats.failed++; \
            printf("  [FAIL] %s (expected: '%s', got: '%s')\n", message, expected, actual); \
        } \
    } while(0)

// Test function type
typedef void (*test_function_t)(void);

// Test registration structure
typedef struct {
    const char* name;
    test_function_t function;
} test_case_t;

// Test suite functions
void test_framework_init(void);
void test_framework_print_summary(void);

// Test suite declarations
extern test_case_t file_operation_tests[];
extern test_case_t directory_operation_tests[];
extern test_case_t path_operation_tests[];

#endif // TEST_FRAMEWORK_H
