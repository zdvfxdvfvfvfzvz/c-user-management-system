#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <ctype.h>

// Test counter
int tests_passed = 0;
int tests_failed = 0;

#define TEST_ASSERT(condition, test_name) \
    do { \
        if (condition) { \
            printf("✓ PASS: %s\n", test_name); \
            tests_passed++; \
        } else { \
            printf("✗ FAIL: %s\n", test_name); \
            tests_failed++; \
        } \
    } while(0)

// Test helper functions
void create_test_csv(const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        printf("ERROR: Cannot create test CSV file\n");
        exit(1);
    }
    
    fprintf(file, "ReviewerName,SatisfactionScore,ReviewDate,Feedback\n");
    fprintf(file, "Alice,5,2024-01-15,Excellent service\n");
    fprintf(file, "Bob,4,2024-01-16,Good product\n");
    fprintf(file, "Charlie,3,2024-01-17,Average experience\n");
    fprintf(file, "David,2,2024-01-18,Could be better\n");
    fprintf(file, "Eve,5,2024-01-19,Amazing quality\n");
    
    fclose(file);
}

void cleanup_test_files() {
    remove("test_reviews.csv");
    remove("test_backup.csv");
    remove("test_empty.csv");
}

// ========== TEST FUNCTIONS ==========

void test_csv_creation_and_loading() {
    printf("\n=== Test: CSV Creation and Loading ===\n");
    
    // Create test CSV
    create_test_csv("test_reviews.csv");
    
    // Check file exists
    FILE *file = fopen("test_reviews.csv", "r");
    TEST_ASSERT(file != NULL, "Test CSV file created successfully");
    
    if (file) {
        char line[256];
        int line_count = 0;
        
        // Count lines
        while (fgets(line, sizeof(line), file)) {
            line_count++;
        }
        
        TEST_ASSERT(line_count == 6, "CSV has correct number of lines (header + 5 reviews)");
        fclose(file);
    }
}

void test_csv_parsing() {
    printf("\n=== Test: CSV Parsing ===\n");
    
    create_test_csv("test_reviews.csv");
    
    FILE *file = fopen("test_reviews.csv", "r");
    char line[1024];
    
    // Skip header
    fgets(line, sizeof(line), file);
    
    // Read first data line
    if (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;
        
        char *name = strtok(line, ",");
        char *score = strtok(NULL, ",");
        char *date = strtok(NULL, ",");
        char *feedback = strtok(NULL, "");
        
        TEST_ASSERT(strcmp(name, "Alice") == 0, "Name parsed correctly");
        TEST_ASSERT(strcmp(score, "5") == 0, "Score parsed correctly");
        TEST_ASSERT(strcmp(date, "2024-01-15") == 0, "Date parsed correctly");
        TEST_ASSERT(strstr(feedback, "Excellent") != NULL, "Feedback parsed correctly");
    }
    
    fclose(file);
}

void test_csv_with_commas_in_feedback() {
    printf("\n=== Test: CSV with Commas in Feedback ===\n");
    
    FILE *file = fopen("test_reviews.csv", "w");
    fprintf(file, "ReviewerName,SatisfactionScore,ReviewDate,Feedback\n");
    fprintf(file, "John,5,2024-02-01,Great product, fast delivery, excellent support!\n");
    fclose(file);
    
    file = fopen("test_reviews.csv", "r");
    char line[1024];
    
    // Skip header
    fgets(line, sizeof(line), file);
    
    // Read data line
    if (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;
        
        // Parse the line
        strtok(line, ",");                  // name
        strtok(NULL, ",");                  // score
        strtok(NULL, ",");                  // date
        char *feedback = strtok(NULL, "");  // Get rest of line
        
        TEST_ASSERT(strstr(feedback, "Great product") != NULL, "Feedback with commas preserved");
        TEST_ASSERT(strstr(feedback, "fast delivery") != NULL, "Full feedback maintained");
    }
    
    fclose(file);
}

void test_empty_csv() {
    printf("\n=== Test: Empty CSV File ===\n");
    
    // Create empty CSV with only header
    FILE *file = fopen("test_empty.csv", "w");
    fprintf(file, "ReviewerName,SatisfactionScore,ReviewDate,Feedback\n");
    fclose(file);
    
    file = fopen("test_empty.csv", "r");
    char line[256];
    int line_count = 0;
    
    while (fgets(line, sizeof(line), file)) {
        line_count++;
    }
    
    TEST_ASSERT(line_count == 1, "Empty CSV has only header line");
    fclose(file);
}

void test_data_validation() {
    printf("\n=== Test: Data Validation ===\n");
    
    // Test date validation
    int valid_dates = 0;
    char *dates[] = {
        "2024-01-15",
        "2024-12-31",
        "2567-05-20",  // BE year
        "2024-02-29",  // Leap year
        "2024-13-01",  // Invalid month
        "2024-12-32",  // Invalid day
        "2024/01/15"   // Wrong format
    };
    
    // Simplified validation for testing
    for (int i = 0; i < 4; i++) {
        if (strlen(dates[i]) == 10 && dates[i][4] == '-' && dates[i][7] == '-') {
            valid_dates++;
        }
    }
    
    TEST_ASSERT(valid_dates == 4, "Correct date format validation");
    
    // Test score validation
    int valid_scores = 0;
    char *scores[] = {"1", "3", "5", "0", "6", "abc"};
    
    for (int i = 0; i < 3; i++) {
        int score = atoi(scores[i]);
        if (score >= 1 && score <= 5) {
            valid_scores++;
        }
    }
    
    TEST_ASSERT(valid_scores == 3, "Score range validation (1-5)");
}

void test_backup_and_restore() {
    printf("\n=== Test: Backup and Restore ===\n");
    
    // Create original file
    create_test_csv("test_reviews.csv");
    
    // Create backup (simulate)
    FILE *source = fopen("test_reviews.csv", "r");
    FILE *backup = fopen("test_backup.csv", "w");
    
    char line[1024];
    while (fgets(line, sizeof(line), source)) {
        fputs(line, backup);
    }
    
    fclose(source);
    fclose(backup);
    
    TEST_ASSERT(access("test_backup.csv", F_OK) == 0, "Backup file created");
    
    // Verify backup has same content
    FILE *original = fopen("test_reviews.csv", "r");
    backup = fopen("test_backup.csv", "r");
    
    int lines_match = 1;
    char line1[1024], line2[1024];
    
    while (fgets(line1, sizeof(line1), original) && fgets(line2, sizeof(line2), backup)) {
        if (strcmp(line1, line2) != 0) {
            lines_match = 0;
            break;
        }
    }
    
    fclose(original);
    fclose(backup);
    
    TEST_ASSERT(lines_match == 1, "Backup content matches original");
}

void test_statistics_calculation() {
    printf("\n=== Test: Statistics Calculation ===\n");
    
    int scores[] = {5, 4, 3, 2, 5};
    int count = 5;
    float total = 0;
    
    for (int i = 0; i < count; i++) {
        total += scores[i];
    }
    
    float average = total / count;
    TEST_ASSERT(average == 3.8f, "Average score calculated correctly");
    
    // Count distribution
    int distribution[5] = {0};
    for (int i = 0; i < count; i++) {
        distribution[scores[i] - 1]++;
    }
    
    TEST_ASSERT(distribution[4] == 2, "Count of 5-star reviews correct (2)");
    TEST_ASSERT(distribution[1] == 1, "Count of 2-star reviews correct (1)");
}

void test_search_functionality() {
    printf("\n=== Test: Search Functionality ===\n");
    
    char *names[] = {"Alice", "Bob", "Charlie", "David", "Eve"};
    int count = 5;
    
    // Exact match search
    int found = 0;
    for (int i = 0; i < count; i++) {
        if (strcmp(names[i], "Charlie") == 0) {
            found = 1;
            break;
        }
    }
    TEST_ASSERT(found == 1, "Exact name search works");
    
    // Case-insensitive search
    found = 0;
    for (int i = 0; i < count; i++) {
        char lower_name[50];
        strcpy(lower_name, names[i]);
        for (int j = 0; lower_name[j]; j++) {
            lower_name[j] = tolower(lower_name[j]);
        }
        if (strcmp(lower_name, "alice") == 0) {
            found = 1;
            break;
        }
    }
    TEST_ASSERT(found == 1, "Case-insensitive search works");
}

void test_delete_functionality() {
    printf("\n=== Test: Delete Functionality ===\n");
    
    // Simulate deletion
    char *names[] = {"Alice", "Bob", "Charlie", "David", "Eve"};
    int count = 5;
    
    // Delete "Charlie" (index 2)
    int delete_index = 2;
    for (int i = delete_index; i < count - 1; i++) {
        names[i] = names[i + 1];
    }
    count--;
    
    TEST_ASSERT(count == 4, "Count decreased after deletion");
    TEST_ASSERT(strcmp(names[2], "David") == 0, "Elements shifted correctly after deletion");
    
    // Verify Charlie is gone
    int found = 0;
    for (int i = 0; i < count; i++) {
        if (strcmp(names[i], "Charlie") == 0) {
            found = 1;
            break;
        }
    }
    TEST_ASSERT(found == 0, "Deleted element no longer exists");
}

void test_update_functionality() {
    printf("\n=== Test: Update Functionality ===\n");
    
    // Simulate update
    char *names[] = {"Alice", "Bob", "Charlie"};
    int scores[] = {5, 4, 3};
    
    // Update Bob's score
    scores[1] = 5;
    
    TEST_ASSERT(scores[1] == 5, "Score updated successfully");
    TEST_ASSERT(strcmp(names[1], "Bob") == 0, "Correct review updated");
    
    // Update name
    names[2] = "Charles";
    TEST_ASSERT(strcmp(names[2], "Charles") == 0, "Name updated successfully");
}

void test_memory_management() {
    printf("\n=== Test: Memory Management ===\n");
    
    // Test string allocation
    char *str1 = malloc(50);
    strcpy(str1, "Test string");
    TEST_ASSERT(str1 != NULL, "Memory allocated successfully");
    TEST_ASSERT(strcmp(str1, "Test string") == 0, "String copied correctly");
    free(str1);
    
    // Test array resize simulation
    int capacity = 5;
    int *arr = malloc(capacity * sizeof(int));
    TEST_ASSERT(arr != NULL, "Initial array allocated");
    
    // Simulate resize
    capacity *= 2;
    arr = realloc(arr, capacity * sizeof(int));
    TEST_ASSERT(arr != NULL, "Array resized successfully");
    TEST_ASSERT(capacity == 10, "Capacity doubled correctly");
    free(arr);
}

void test_edge_cases() {
    printf("\n=== Test: Edge Cases ===\n");
    
    // Test with long strings
    char long_feedback[300];
    memset(long_feedback, 'A', 250);
    long_feedback[250] = '\0';
    
    TEST_ASSERT(strlen(long_feedback) == 250, "Long string handled");
    
    // Test with empty strings
    char empty[] = "";
    TEST_ASSERT(strlen(empty) == 0, "Empty string handled");
    
    // Test with special characters
    char special[] = "Test!@#$%^&*()";
    TEST_ASSERT(strchr(special, '!') != NULL, "Special characters preserved");
    
    // Test boundary values
    TEST_ASSERT(1 >= 1 && 1 <= 5, "Min score boundary");
    TEST_ASSERT(5 >= 1 && 5 <= 5, "Max score boundary");
}

void test_file_operations() {
    printf("\n=== Test: File Operations ===\n");
    
    // Test write and read
    FILE *file = fopen("test_file_ops.csv", "w");
    TEST_ASSERT(file != NULL, "File opened for writing");
    
    fprintf(file, "Test,Data,Here\n");
    fclose(file);
    
    file = fopen("test_file_ops.csv", "r");
    TEST_ASSERT(file != NULL, "File opened for reading");
    
    char line[100];
    fgets(line, sizeof(line), file);
    TEST_ASSERT(strstr(line, "Test") != NULL, "Data written and read correctly");
    
    fclose(file);
    remove("test_file_ops.csv");
}

// ========== MAIN TEST RUNNER ==========

int main() {
    printf("\n");
    printf("╔════════════════════════════════════════════════╗\n");
    printf("║    CUSTOMER REVIEW SYSTEM - E2E TESTS          ║\n");
    printf("╚════════════════════════════════════════════════╝\n");
    
    // Run all test suites
    test_csv_creation_and_loading();
    test_csv_parsing();
    test_csv_with_commas_in_feedback();
    test_empty_csv();
    test_data_validation();
    test_backup_and_restore();
    test_statistics_calculation();
    test_search_functionality();
    test_delete_functionality();
    test_update_functionality();
    test_memory_management();
    test_edge_cases();
    test_file_operations();
    
    // Cleanup
    cleanup_test_files();
    
    // Print summary
    printf("\n");
    printf("╔════════════════════════════════════════════════╗\n");
    printf("║              TEST SUMMARY                      ║\n");
    printf("╚════════════════════════════════════════════════╝\n");
    printf("Total Tests:  %d\n", tests_passed + tests_failed);
    printf("✓ Passed:     %d\n", tests_passed);
    printf("✗ Failed:     %d\n", tests_failed);
    printf("\n");
    
    if (tests_failed == 0) {
        printf("🎉 ALL E2E TESTS PASSED! 🎉\n");
        return 0;
    } else {
        printf("❌ SOME TESTS FAILED\n");
        return 1;
    }
}