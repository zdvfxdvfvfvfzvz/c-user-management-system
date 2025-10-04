#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// =================== WHAT IS UNIT TESTING? ===================

/*
Unit Testing = การทดสอบ functions ทีละตัว แยกจากระบบใหญ่

Example:
- Test parseScore() function with valid input (1-5)
- Test parseScore() function with invalid input ("abc")
- Test addReview() function with valid data
- Test addReview() function with NULL data
*/

// =================== YOUR CSV FUNCTIONS TO TEST ===================

// Function 1: Parse and validate satisfaction score
int parseScore(const char* score_str) {
    if (!score_str || strlen(score_str) == 0) {
        return -1;  // Error: NULL or empty
    }
    
    char *endptr;
    long score_long = strtol(score_str, &endptr, 10);
    
    if (*endptr != '\0') {
        return -1;  // Error: Not a number
    }
    
    if (score_long < 1 || score_long > 5) {
        return -1;  // Error: Out of range
    }
    
    return (int)score_long;
}

// Function 2: Validate date format (YYYY-MM-DD)
int isValidDate(const char* date_str) {
    if (!date_str || strlen(date_str) != 10) {
        return 0;  // Invalid length
    }
    
    // Check format: YYYY-MM-DD
    if (date_str[4] != '-' || date_str[7] != '-') {
        return 0;  // Missing dashes
    }
    
    // Check if year, month, day are numbers
    for (int i = 0; i < 10; i++) {
        if (i == 4 || i == 7) continue;  // Skip dashes
        if (date_str[i] < '0' || date_str[i] > '9') {
            return 0;  // Not a digit
        }
    }
    
    return 1;  // Valid
}

// Function 3: Clean and validate reviewer name
char* cleanReviewerName(const char* name) {
    if (!name || strlen(name) == 0) {
        return NULL;
    }
    
    // Remove leading/trailing spaces
    const char* start = name;
    while (*start == ' ') start++;
    
    if (*start == '\0') {
        return NULL;  // Only spaces
    }
    
    const char* end = start + strlen(start) - 1;
    while (end > start && *end == ' ') end--;
    
    size_t clean_len = end - start + 1;
    char* clean_name = malloc(clean_len + 1);
    if (!clean_name) return NULL;
    
    strncpy(clean_name, start, clean_len);
    clean_name[clean_len] = '\0';
    
    return clean_name;
}

// =================== SIMPLE UNIT TEST FRAMEWORK ===================

int test_count = 0;
int test_passed = 0;
int test_failed = 0;

void test_assert(int condition, const char* test_name) {
    test_count++;
    if (condition) {
        printf("✅ PASS: %s\n", test_name);
        test_passed++;
    } else {
        printf("❌ FAIL: %s\n", test_name);
        test_failed++;
    }
}

void test_assert_equal_int(int expected, int actual, const char* test_name) {
    test_count++;
    if (expected == actual) {
        printf("✅ PASS: %s\n", test_name);
        test_passed++;
    } else {
        printf("❌ FAIL: %s - Expected: %d, Got: %d\n", test_name, expected, actual);
        test_failed++;
    }
}

void test_assert_equal_str(const char* expected, const char* actual, const char* test_name) {
    test_count++;
    if (expected == NULL && actual == NULL) {
        printf("✅ PASS: %s\n", test_name);
        test_passed++;
    } else if (expected == NULL || actual == NULL) {
        printf("❌ FAIL: %s - One is NULL\n", test_name);
        test_failed++;
    } else if (strcmp(expected, actual) == 0) {
        printf("✅ PASS: %s\n", test_name);
        test_passed++;
    } else {
        printf("❌ FAIL: %s - Expected: '%s', Got: '%s'\n", test_name, expected, actual);
        test_failed++;
    }
}

// =================== UNIT TESTS FOR YOUR FUNCTIONS ===================

void test_parseScore() {
    printf("\n=== Testing parseScore() Function ===\n");
    
    // Test valid scores
    test_assert_equal_int(1, parseScore("1"), "parseScore with valid score 1");
    test_assert_equal_int(3, parseScore("3"), "parseScore with valid score 3");
    test_assert_equal_int(5, parseScore("5"), "parseScore with valid score 5");
    
    // Test invalid scores
    test_assert_equal_int(-1, parseScore("0"), "parseScore with score 0 (too low)");
    test_assert_equal_int(-1, parseScore("6"), "parseScore with score 6 (too high)");
    test_assert_equal_int(-1, parseScore("10"), "parseScore with score 10 (too high)");
    
    // Test invalid formats
    test_assert_equal_int(-1, parseScore("abc"), "parseScore with non-numeric input");
    test_assert_equal_int(-1, parseScore(""), "parseScore with empty string");
    test_assert_equal_int(-1, parseScore(NULL), "parseScore with NULL input");
    test_assert_equal_int(-1, parseScore("3.5"), "parseScore with decimal");
    test_assert_equal_int(-1, parseScore("3abc"), "parseScore with mixed input");
    test_assert_equal_int(-1, parseScore(" 3 "), "parseScore with spaces");
}

void test_isValidDate() {
    printf("\n=== Testing isValidDate() Function ===\n");
    
    // Test valid dates
    test_assert_equal_int(1, isValidDate("2025-08-01"), "Valid date format");
    test_assert_equal_int(1, isValidDate("2025-12-31"), "Valid date end of year");
    test_assert_equal_int(1, isValidDate("2000-01-01"), "Valid date Y2K");
    
    // Test invalid formats
    test_assert_equal_int(0, isValidDate("2025/08/01"), "Invalid separator");
    test_assert_equal_int(0, isValidDate("25-08-01"), "Invalid year format");
    test_assert_equal_int(0, isValidDate("2025-8-1"), "Missing leading zeros");
    test_assert_equal_int(0, isValidDate("2025-08-1"), "Missing day leading zero");
    
    // Test edge cases
    test_assert_equal_int(0, isValidDate(""), "Empty date string");
    test_assert_equal_int(0, isValidDate(NULL), "NULL date string");
    test_assert_equal_int(0, isValidDate("2025-08-011"), "Too long");
    test_assert_equal_int(0, isValidDate("2025-08-1"), "Too short");
    test_assert_equal_int(0, isValidDate("abcd-ef-gh"), "Non-numeric date");
}

void test_cleanReviewerName() {
    printf("\n=== Testing cleanReviewerName() Function ===\n");
    
    // Test normal names
    char *result1 = cleanReviewerName("John Doe");
    test_assert_equal_str("John Doe", result1, "Clean normal name");
    if (result1) free(result1);
    
    char *result2 = cleanReviewerName("Jane Smith");
    test_assert_equal_str("Jane Smith", result2, "Clean another normal name");
    if (result2) free(result2);
    
    // Test names with spaces
    char *result3 = cleanReviewerName("  John Doe  ");
    test_assert_equal_str("John Doe", result3, "Clean name with leading/trailing spaces");
    if (result3) free(result3);
    
    char *result4 = cleanReviewerName(" Sarah Wilson ");
    test_assert_equal_str("Sarah Wilson", result4, "Clean name with single spaces");
    if (result4) free(result4);
    
    // Test edge cases
    char *result5 = cleanReviewerName("");
    test_assert_equal_str(NULL, result5, "Clean empty string");
    
    char *result6 = cleanReviewerName("   ");
    test_assert_equal_str(NULL, result6, "Clean spaces-only string");
    
    char *result7 = cleanReviewerName(NULL);
    test_assert_equal_str(NULL, result7, "Clean NULL input");
}

// =================== INTEGRATION TESTS ===================

void test_csvParsing() {
    printf("\n=== Testing CSV Line Parsing ===\n");
    
    // Test complete CSV parsing
    char csv_line[] = "John Doe,5,2025-08-01,Excellent service";
    
    char line_copy[256];
    strcpy(line_copy, csv_line);
    
    char *name = strtok(line_copy, ",");
    char *score_str = strtok(NULL, ",");
    char *date = strtok(NULL, ",");
    char *feedback = strtok(NULL, ",");
    
    // Test each parsed component
    test_assert(name != NULL, "CSV parsing - name extracted");
    test_assert(score_str != NULL, "CSV parsing - score extracted");
    test_assert(date != NULL, "CSV parsing - date extracted");
    test_assert(feedback != NULL, "CSV parsing - feedback extracted");
    
    if (name && score_str && date && feedback) {
        test_assert_equal_str("John Doe", name, "Parsed name correct");
        test_assert_equal_int(5, parseScore(score_str), "Parsed score valid");
        test_assert_equal_int(1, isValidDate(date), "Parsed date valid");
        test_assert_equal_str("Excellent service", feedback, "Parsed feedback correct");
    }
}

void test_errorCases() {
    printf("\n=== Testing Error Cases ===\n");
    
    // Test malformed CSV
    char bad_csv[] = "John Doe,abc,invalid-date,";
    char bad_copy[256];
    strcpy(bad_copy, bad_csv);
    
    char *name = strtok(bad_copy, ",");
    char *score_str = strtok(NULL, ",");
    char *date = strtok(NULL, ",");
    char *feedback = strtok(NULL, ",");
    
    test_assert_equal_int(-1, parseScore(score_str), "Invalid score detected");
    test_assert_equal_int(0, isValidDate(date), "Invalid date detected");
    test_assert(feedback != NULL && strlen(feedback) == 0, "Empty feedback detected");
}

// =================== PERFORMANCE TESTS ===================

void test_performance() {
    printf("\n=== Performance Tests ===\n");
    
    clock_t start, end;
    
    // Test parseScore performance
    start = clock();
    for (int i = 0; i < 100000; i++) {
        parseScore("3");
    }
    end = clock();
    
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("parseScore 100,000 calls: %.4f seconds\n", time_taken);
    test_assert(time_taken < 1.0, "parseScore performance acceptable");
    
    // Test date validation performance
    start = clock();
    for (int i = 0; i < 100000; i++) {
        isValidDate("2025-08-01");
    }
    end = clock();
    
    time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("isValidDate 100,000 calls: %.4f seconds\n", time_taken);
    test_assert(time_taken < 1.0, "isValidDate performance acceptable");
}

// =================== TEST RUNNER ===================

void runAllTests() {
    printf("🚀 Starting Unit Tests for CSV Review System\n");
    printf("=" * 50);
    
    // Reset counters
    test_count = 0;
    test_passed = 0;
    test_failed = 0;
    
    // Run all test suites
    test_parseScore();
    test_isValidDate();
    test_cleanReviewerName();
    test_csvParsing();
    test_errorCases();
    test_performance();
    
    // Print summary
    printf("\n" "=" * 50);
    printf("\n🏁 TEST SUMMARY\n");
    printf("Total Tests: %d\n", test_count);
    printf("✅ Passed: %d\n", test_passed);
    printf("❌ Failed: %d\n", test_failed);
    printf("Success Rate: %.1f%%\n", (float)test_passed / test_count * 100);
    
    if (test_failed == 0) {
        printf("\n🎉 ALL TESTS PASSED! Your code is solid! 🎉\n");
    } else {
        printf("\n⚠️  Some tests failed. Please fix the issues above.\n");
    }
}

// =================== HOW TO ADD TO YOUR PROJECT ===================

void howToIntegrateWithYourProject() {
    printf("\n=== HOW TO ADD UNIT TESTS TO YOUR PROJECT ===\n\n");
    
    printf("1. CREATE TEST FILE:\n");
    printf("   Create: test_csv_system.c\n");
    printf("   Copy the test framework and your functions\n\n");
    
    printf("2. COMPILE AND RUN:\n");
    printf("   gcc -o test_runner test_csv_system.c\n");
    printf("   ./test_runner\n\n");
    
    printf("3. ADD TO YOUR MAIN PROJECT:\n");
    printf("   In your main.c, add:\n");
    printf("   #ifdef TESTING\n");
    printf("   extern void runAllTests();\n");
    printf("   if (argc > 1 && strcmp(argv[1], \"--test\") == 0) {\n");
    printf("       runAllTests();\n");
    printf("       return 0;\n");
    printf("   }\n");
    printf("   #endif\n\n");
    
    printf("4. COMPILE WITH TESTING:\n");
    printf("   gcc -DTESTING -o csv_program main.c test_csv_system.c\n");
    printf("   ./csv_program --test\n\n");
    
    printf("5. WHAT TO TEST IN YOUR PROJECT:\n");
    printf("   ✅ parseScore() - validate satisfaction scores\n");
    printf("   ✅ isValidDate() - validate date format\n");
    printf("   ✅ addReview() - add valid/invalid reviews\n");
    printf("   ✅ searchReview() - search by name/score\n");
    printf("   ✅ updateReview() - modify existing reviews\n");
    printf("   ✅ deleteReview() - remove reviews\n");
    printf("   ✅ CSV parsing - handle malformed data\n");
    printf("   ✅ Memory management - no leaks\n\n");
}

int main() {
    int choice;
    
    printf("C Unit Testing Demo\n");
    printf("1. Run all unit tests\n");
    printf("2. Show integration guide\n");
    printf("Enter choice: ");
    scanf("%d", &choice);
    
    switch(choice) {
        case 1:
            runAllTests();
            break;
        case 2:
            howToIntegrateWithYourProject();
            break;
        default:
            printf("Running all tests by default...\n");
            runAllTests();
    }
    
    return 0;
}