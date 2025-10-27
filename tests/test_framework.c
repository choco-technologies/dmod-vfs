#include "test_framework.h"
#include <stdio.h>

// Global test statistics
test_stats_t g_test_stats = {0, 0, 0};

// Initialize test framework
void test_framework_init(void)
{
    g_test_stats.total = 0;
    g_test_stats.passed = 0;
    g_test_stats.failed = 0;
}

// Print test summary
void test_framework_print_summary(void)
{
    printf("\n");
    printf("=========================================\n");
    printf("Test Summary:\n");
    printf("  Total:  %d\n", g_test_stats.total);
    printf("  Passed: %d\n", g_test_stats.passed);
    printf("  Failed: %d\n", g_test_stats.failed);
    
    if (g_test_stats.failed == 0 && g_test_stats.total > 0) {
        printf("\n  ALL TESTS PASSED!\n");
    } else if (g_test_stats.failed > 0) {
        printf("\n  SOME TESTS FAILED!\n");
    }
    printf("=========================================\n");
}
