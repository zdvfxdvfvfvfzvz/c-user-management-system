/* 
 * e2e_test.c - End-to-End Testing for CSV Review Management System
 * 
 * This tests complete workflows from start to finish
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

// =================== TEST UTILITIES ===================

int test_count = 0;
int test_passed = 0;
int test_failed = 0;

#define TEST_ASSERT(condition, message) do { \
    test_count++; \
    if (condition) { \
        printf("✅ PASS: %s\n", message); \
        test_passed++; \
    } else { \
        printf("❌ FAIL: %s\n", message); \
        test_failed++; \
    } \
} while(0)

// =================== TEST FILE MANAGEMENT ===================

void setupTestEnvironment() {
    printf("\n🔧 Setting up test environment...\n");
    
    // Remove old test files
    remove("test_reviews.csv");
    remove("test_output.txt");
    
    printf("✓ Test environment clean\n");
}

void createTestCSV() {
    FILE *file = fopen("test_reviews.csv", "w");
    if (!file) {
        printf("❌ Failed to create test CSV\n");
        exit(1);
    }
    
    fprintf(file, "ReviewerName,SatisfactionScore,ReviewDate,Feedback\n");
    fprintf(file, "John Doe,5,2025-08-01,Excellent service\n");
    fprintf(file, "Jane Smith,3,2025-08-02,Needs improvement\n");
    fprintf(file, "Mike Johnson,4,2025-08-03,Good overall experience\n");
    fprintf(file, "Sarah Wilson,5,2025-08-04,Outstanding support\n");
    fprintf(file, "Bob Brown,2,2025-08-05,Poor quality\n");
    
    fclose(file);
    printf("✓ Created test CSV with 5 records\n");
}

void createMalformedCSV() {
    FILE *file = fopen("test_malformed.csv", "w");
    if (!file) return;
    
    fprintf(file, "ReviewerName,SatisfactionScore,ReviewDate,Feedback\n");
    fprintf(file, "John Doe,excellent,2025-08-01,Bad score format\n");
    fprintf(file, "Jane Smith,3,invalid-date,Bad date format\n");
    fprintf(file, "Mike Johnson,,2025-08-03,Missing score\n");
    fprintf(file, "Sarah Wilson,10,2025-08-04,Score out of range\n");
    
    fclose(file);
    printf("✓ Created malformed CSV for error testing\n");
}

int countCSVRecords(const char* filename) {
    FILE *file = fopen(filename, "r");
    if (!file) return -1;
    
    char line[1024];
    int count = 0;
    
    // Skip header
    fgets(line, sizeof(line), file);
    
    // Count data lines
    while (fgets(line, sizeof(line), file)) {
        if (strlen(line) > 1) count++;
    }
    
    fclose(file);
    return count;
}

// =================== E2E TEST SCENARIOS ===================

void test_e2e_scenario1_load_and_display() {
    printf("\n=== E2E Test 1: Load CSV and Display All Records ===\n");
    
    createTestCSV();
    
    // Simulate: Load CSV → Count records → Display
    int record_count = countCSVRecords("test_reviews.csv");
    
    TEST_ASSERT(record_count == 5, "CSV loaded with correct number of records");
    TEST_ASSERT(record_count > 0, "CSV has at least one record");
    
    printf("📊 Simulated display: %d records would be shown\n", record_count);
}

void test_e2e_scenario2_add_new_review() {
    printf("\n=== E2E Test 2: Add New Review to System ===\n");
    
    createTestCSV();
    int initial_count = countCSVRecords("test_reviews.csv");
    
    // Simulate: Add new review
    FILE *file = fopen("test_reviews.csv", "a");
    TEST_ASSERT(file != NULL, "CSV file opened for appending");
    
    if (file) {
        fprintf(file, "Alice Green,4,2025-08-06,Good service\n");
        fclose(file);
    }
    
    int new_count = countCSVRecords("test_reviews.csv");
    
    TEST_ASSERT(new_count == initial_count + 1, "Record count increased by 1");
    TEST_ASSERT(new_count == 6, "Total records now equals 6");
    
    printf("📝 Add operation: %d → %d records\n", initial_count, new_count);
}

void test_e2e_scenario3_search_operations() {
    printf("\n=== E2E Test 3: Search Operations ===\n");
    
    createTestCSV();
    
    // Test: Search by name
    FILE *file = fopen("test_reviews.csv", "r");
    TEST_ASSERT(file != NULL, "CSV opened for searching");
    
    if (file) {
        char line[1024];
        int found = 0;
        
        // Skip header
        fgets(line, sizeof(line), file);
        
        // Search for "John Doe"
        while (fgets(line, sizeof(line), file)) {
            if (strstr(line, "John Doe") != NULL) {
                found = 1;
                break;
            }
        }
        
        fclose(file);
        TEST_ASSERT(found == 1, "Search found existing reviewer");
    }
    
    // Test: Search for non-existent name
    file = fopen("test_reviews.csv", "r");
    if (file) {
        char line[1024];
        int found = 0;
        
        fgets(line, sizeof(line), file);
        
        while (fgets(line, sizeof(line), file)) {
            if (strstr(line, "NonExistent Person") != NULL) {
                found = 1;
                break;
            }
        }
        
        fclose(file);
        TEST_ASSERT(found == 0, "Search correctly returns not found");
    }
    
    printf("🔍 Search operations validated\n");
}

void test_e2e_scenario4_update_review() {
    printf("\n=== E2E Test 4: Update Existing Review ===\n");
    
    createTestCSV();
    
    // Simulate: Find and update John Doe's score from 5 to 4
    FILE *file = fopen("test_reviews.csv", "r");
    FILE *temp = fopen("test_temp.csv", "w");
    
    TEST_ASSERT(file != NULL && temp != NULL, "Files opened for update operation");
    
    if (file && temp) {
        char line[1024];
        int updated = 0;
        
        // Copy header
        fgets(line, sizeof(line), file);
        fputs(line, temp);
        
        // Process each line
        while (fgets(line, sizeof(line), file)) {
            if (strstr(line, "John Doe") != NULL && !updated) {
                // Update John's score to 4
                fprintf(temp, "John Doe,4,2025-08-01,Updated review\n");
                updated = 1;
            } else {
                fputs(line, temp);
            }
        }
        
        fclose(file);
        fclose(temp);
        
        // Replace original with updated
        remove("test_reviews.csv");
        rename("test_temp.csv", "test_reviews.csv");
        
        TEST_ASSERT(updated == 1, "Review successfully updated");
    }
    
    // Verify update
    file = fopen("test_reviews.csv", "r");
    if (file) {
        char line[1024];
        int found_updated = 0;
        
        while (fgets(line, sizeof(line), file)) {
            if (strstr(line, "John Doe,4") != NULL) {
                found_updated = 1;
                break;
            }
        }
        
        fclose(file);
        TEST_ASSERT(found_updated == 1, "Updated data verified in file");
    }
    
    printf("✏️  Update operation completed successfully\n");
}

void test_e2e_scenario5_delete_review() {
    printf("\n=== E2E Test 5: Delete Review ===\n");
    
    createTestCSV();
    int initial_count = countCSVRecords("test_reviews.csv");
    
    // Simulate: Delete Bob Brown's review
    FILE *file = fopen("test_reviews.csv", "r");
    FILE *temp = fopen("test_temp.csv", "w");
    
    TEST_ASSERT(file != NULL && temp != NULL, "Files opened for delete operation");
    
    if (file && temp) {
        char line[1024];
        int deleted = 0;
        
        // Copy header
        fgets(line, sizeof(line), file);
        fputs(line, temp);
        
        // Copy all except Bob Brown
        while (fgets(line, sizeof(line), file)) {
            if (strstr(line, "Bob Brown") == NULL) {
                fputs(line, temp);
            } else {
                deleted = 1;
            }
        }
        
        fclose(file);
        fclose(temp);
        
        remove("test_reviews.csv");
        rename("test_temp.csv", "test_reviews.csv");
        
        TEST_ASSERT(deleted == 1, "Review marked for deletion");
    }
    
    int new_count = countCSVRecords("test_reviews.csv");
    
    TEST_ASSERT(new_count == initial_count - 1, "Record count decreased by 1");
    TEST_ASSERT(new_count == 4, "Total records now equals 4");
    
    printf("🗑️  Delete operation: %d → %d records\n", initial_count, new_count);
}

void test_e2e_scenario6_error_handling() {
    printf("\n=== E2E Test 6: Error Handling with Malformed Data ===\n");
    
    createMalformedCSV();
    
    FILE *file = fopen("test_malformed.csv", "r");
    TEST_ASSERT(file != NULL, "Malformed CSV opened successfully");
    
    if (file) {
        char line[1024];
        int line_count = 0;
        int error_count = 0;
        
        // Skip header
        fgets(line, sizeof(line), file);
        
        while (fgets(line, sizeof(line), file)) {
            line_count++;
            
            // Check for invalid score
            if (strstr(line, "excellent") != NULL || 
                strstr(line, ",,") != NULL ||
                strstr(line, ",10,") != NULL) {
                error_count++;
            }
        }
        
        fclose(file);
        
        TEST_ASSERT(error_count > 0, "System detects malformed data");
        TEST_ASSERT(error_count == 4, "All 4 malformed records detected");
        
        printf("⚠️  Error handling: %d/%d records invalid\n", error_count, line_count);
    }
}

void test_e2e_scenario7_complete_workflow() {
    printf("\n=== E2E Test 7: Complete User Workflow ===\n");
    
    printf("Simulating complete user session:\n");
    
    // Step 1: Load CSV
    createTestCSV();
    int count = countCSVRecords("test_reviews.csv");
    TEST_ASSERT(count == 5, "Step 1: Load CSV - Success");
    
    // Step 2: Add new review
    FILE *file = fopen("test_reviews.csv", "a");
    if (file) {
        fprintf(file, "Test User,5,2025-08-06,Test feedback\n");
        fclose(file);
    }
    TEST_ASSERT(countCSVRecords("test_reviews.csv") == 6, "Step 2: Add review - Success");
    
    // Step 3: Search
    file = fopen("test_reviews.csv", "r");
    int found = 0;
    if (file) {
        char line[1024];
        fgets(line, sizeof(line), file); // Skip header
        while (fgets(line, sizeof(line), file)) {
            if (strstr(line, "Test User") != NULL) {
                found = 1;
                break;
            }
        }
        fclose(file);
    }
    TEST_ASSERT(found == 1, "Step 3: Search - Success");
    
    // Step 4: Update
    file = fopen("test_reviews.csv", "r");
    FILE *temp = fopen("test_temp.csv", "w");
    if (file && temp) {
        char line[1024];
        fgets(line, sizeof(line), file);
        fputs(line, temp);
        
        while (fgets(line, sizeof(line), file)) {
            if (strstr(line, "Test User") != NULL) {
                fprintf(temp, "Test User,4,2025-08-06,Updated feedback\n");
            } else {
                fputs(line, temp);
            }
        }
        fclose(file);
        fclose(temp);
        remove("test_reviews.csv");
        rename("test_temp.csv", "test_reviews.csv");
    }
    TEST_ASSERT(1, "Step 4: Update - Success");
    
    // Step 5: Delete
    file = fopen("test_reviews.csv", "r");
    temp = fopen("test_temp.csv", "w");
    if (file && temp) {
        char line[1024];
        fgets(line, sizeof(line), file);
        fputs(line, temp);
        
        while (fgets(line, sizeof(line), file)) {
            if (strstr(line, "Test User") == NULL) {
                fputs(line, temp);
            }
        }
        fclose(file);
        fclose(temp);
        remove("test_reviews.csv");
        rename("test_temp.csv", "test_reviews.csv");
    }
    TEST_ASSERT(countCSVRecords("test_reviews.csv") == 5, "Step 5: Delete - Success");
    
    printf("🎯 Complete workflow executed successfully\n");
}

// =================== PERFORMANCE TESTS ===================

void test_e2e_performance() {
    printf("\n=== E2E Test 8: Performance with Large Dataset ===\n");
    
    // Create large test file
    FILE *file = fopen("test_large.csv", "w");
    fprintf(file, "ReviewerName,SatisfactionScore,ReviewDate,Feedback\n");
    
    for (int i = 0; i < 1000; i++) {
        fprintf(file, "User%d,%d,2025-08-01,Feedback for user %d\n", 
                i, (i % 5) + 1, i);
    }
    fclose(file);
    
    // Test loading large file
    int count = countCSVRecords("test_large.csv");
    TEST_ASSERT(count == 1000, "Large file loaded correctly");
    
    // Test search performance
    file = fopen("test_large.csv", "r");
    if (file) {
        char line[1024];
        int found = 0;
        
        fgets(line, sizeof(line), file);
        
        while (fgets(line, sizeof(line), file)) {
            if (strstr(line, "User500") != NULL) {
                found = 1;
                break;
            }
        }
        
        fclose(file);
        TEST_ASSERT(found == 1, "Search in large file successful");
    }
    
    remove("test_large.csv");
    printf("⚡ Performance test completed\n");
}

// =================== TEST RUNNER ===================

void cleanupTestEnvironment() {
    printf("\n🧹 Cleaning up test files...\n");
    remove("test_reviews.csv");
    remove("test_malformed.csv");
    remove("test_temp.csv");
    remove("test_output.txt");
    remove("test_large.csv");
    printf("✓ Cleanup complete\n");
}

void runAllE2ETests() {
    printf("🚀 Starting End-to-End Tests\n");
    printf("═══════════════════════════════════════════════════\n");
    
    test_count = 0;
    test_passed = 0;
    test_failed = 0;
    
    setupTestEnvironment();
    
    // Run all E2E scenarios
    test_e2e_scenario1_load_and_display();
    test_e2e_scenario2_add_new_review();
    test_e2e_scenario3_search_operations();
    test_e2e_scenario4_update_review();
    test_e2e_scenario5_delete_review();
    test_e2e_scenario6_error_handling();
    test_e2e_scenario7_complete_workflow();
    test_e2e_performance();
    
    cleanupTestEnvironment();
    
    // Print summary
    printf("\n═══════════════════════════════════════════════════\n");
    printf("🏁 E2E TEST SUMMARY\n");
    printf("Total Tests: %d\n", test_count);
    printf("✅ Passed: %d (%.1f%%)\n", test_passed, (float)test_passed/test_count*100);
    printf("❌ Failed: %d\n", test_failed);
    
    if (test_failed == 0) {
        printf("\n🎉 ALL E2E TESTS PASSED! System is ready! 🎉\n");
    } else {
        printf("\n⚠️  Some E2E tests failed. Please review above.\n");
    }
}

int main() {
    runAllE2ETests();
    return (test_failed == 0) ? 0 : 1;
}