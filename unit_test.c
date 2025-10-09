#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

// Test counter
int tests_passed = 0;
int tests_failed = 0;

// Macro for test assertions
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

// Copy of functions from main.c that we need to test
int min3(int a, int b, int c) {
    int min = a;
    if (b < min) min = b;
    if (c < min) min = c;
    return min;
}

int editDistance(const char* str1, const char* str2) {
    if (!str1 || !str2) return 999;

    int len1 = strlen(str1);
    int len2 = strlen(str2);
    
    int **dp = (int**)malloc((len1 + 1) * sizeof(int*));
    if (!dp) {
        printf("Memory allocation failed in editDistance!\n");
        return 999;
    }

    for (int i = 0; i <= len1; i++) {
        dp[i] = (int*)malloc((len2 + 1) * sizeof(int));
        if (!dp[i]) {
            for (int j = 0; j < i; j++) {
                free(dp[j]);
            }
            free(dp);
            printf("Memory allocation failed in editDistance!\n");
            return 999;
        }
    }
    
    for (int i = 0; i <= len1; i++) {
        dp[i][0] = i;
    }

    for (int j = 0; j <= len2; j++) {
        dp[0][j] = j;
    }

    for (int i = 1; i <= len1; i++) {
        for (int j = 1; j <= len2; j++) {
            if (tolower(str1[i-1]) == tolower(str2[j-1])) {
                dp[i][j] = dp[i-1][j-1];
            } else {
                dp[i][j] = 1 + min3(
                    dp[i-1][j],
                    dp[i][j-1],
                    dp[i-1][j-1]
                );
            }
        }
    }

    int result = dp[len1][len2];

    for (int i = 0; i <= len1; i++) {
        free(dp[i]);
    }
    free(dp);

    return result;
}

char* toLowerCase(const char *str) {
    if (!str) return NULL;

    char* lower = malloc(strlen(str) + 1);
    if (!lower) return NULL;

    for (int i = 0; str[i]; i++) {
        lower[i] = tolower(str[i]);
    }
    lower[strlen(str)] = '\0';
    return lower;
}

void trim_whitespace(char *str) {
    if (!str) return;
    
    char *start = str;
    while (*start && isspace(*start)) start++;
    
    char *end = start + strlen(start) - 1;
    while (end > start && isspace(*end)) end--;
    
    size_t len = end - start + 1;
    memmove(str, start, len);
    str[len] = '\0';
}

int is_valid_date(const char *date_str) {
    if (!date_str || strlen(date_str) != 10) return 0;
    if (date_str[4] != '-' || date_str[7] != '-') return 0;
    
    for (int i = 0; i < 10; i++) {
        if (i == 4 || i == 7) continue;
        if (date_str[i] < '0' || date_str[i] > '9') return 0;
    }
    
    int year = (date_str[0]-'0')*1000 + (date_str[1]-'0')*100 + 
               (date_str[2]-'0')*10 + (date_str[3]-'0');
    int month = (date_str[5]-'0')*10 + (date_str[6]-'0');
    int day = (date_str[8]-'0')*10 + (date_str[9]-'0');
    
    int valid_year = 0;
    if (year >= 2400 && year <= 2700) {
        valid_year = 1;
    } else if (year >= 1900 && year <= 2100) {
        valid_year = 1;
    }
    
    if (!valid_year) return 0;
    
    if (month < 1 || month > 12) return 0;
    if (day < 1 || day > 31) return 0;
    
    int days_in_month[] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (day > days_in_month[month - 1]) return 0;
    
    return 1;
}

int parseScore(const char* score_str) {
    if (!score_str || strlen(score_str) == 0) {
        return -1;
    }

    char *endptr;
    long score_long = strtol(score_str, &endptr, 10);

    if (*endptr != '\0' || score_long < 1 || score_long > 5) {
        return -1;
    }

    return (int)score_long;
}

char* allocate_string(const char *str) {
    if (!str) return NULL;

    char *new_str = (char*)malloc(strlen(str) + 1);
    if (!new_str) {
        printf("Memory allocation failed for string\n");
        exit(1);
    }
    strcpy(new_str, str);
    return new_str;
}

// ========== TEST FUNCTIONS ==========

void test_min3() {
    printf("\n=== Testing min3() ===\n");
    
    TEST_ASSERT(min3(1, 2, 3) == 1, "min3(1,2,3) should return 1");
    TEST_ASSERT(min3(3, 2, 1) == 1, "min3(3,2,1) should return 1");
    TEST_ASSERT(min3(2, 1, 3) == 1, "min3(2,1,3) should return 1");
    TEST_ASSERT(min3(5, 5, 5) == 5, "min3(5,5,5) should return 5");
    TEST_ASSERT(min3(-1, 0, 1) == -1, "min3(-1,0,1) should return -1");
    TEST_ASSERT(min3(0, 0, 0) == 0, "min3(0,0,0) should return 0");
}

void test_editDistance() {
    printf("\n=== Testing editDistance() ===\n");
    
    TEST_ASSERT(editDistance("kitten", "sitting") == 3, "kitten -> sitting = 3 edits");
    TEST_ASSERT(editDistance("john", "jhon") == 2, "john -> jhon = 2 edits");
    TEST_ASSERT(editDistance("sarah", "sara") == 1, "sarah -> sara = 1 edit");
    TEST_ASSERT(editDistance("hello", "hello") == 0, "hello -> hello = 0 edits (identical)");
    TEST_ASSERT(editDistance("", "") == 0, "empty -> empty = 0 edits");
    TEST_ASSERT(editDistance("abc", "") == 3, "abc -> empty = 3 edits");
    TEST_ASSERT(editDistance("", "xyz") == 3, "empty -> xyz = 3 edits");
    TEST_ASSERT(editDistance("HELLO", "hello") == 0, "Case insensitive: HELLO = hello");
    TEST_ASSERT(editDistance("John", "jon") == 1, "John -> jon = 1 edit");
    TEST_ASSERT(editDistance(NULL, "test") == 999, "NULL pointer should return 999");
    TEST_ASSERT(editDistance("test", NULL) == 999, "NULL pointer should return 999");
}

void test_toLowerCase() {
    printf("\n=== Testing toLowerCase() ===\n");
    
    char *result;
    
    result = toLowerCase("HELLO");
    TEST_ASSERT(strcmp(result, "hello") == 0, "HELLO -> hello");
    free(result);
    
    result = toLowerCase("World123");
    TEST_ASSERT(strcmp(result, "world123") == 0, "World123 -> world123");
    free(result);
    
    result = toLowerCase("MiXeD CaSe");
    TEST_ASSERT(strcmp(result, "mixed case") == 0, "MiXeD CaSe -> mixed case");
    free(result);
    
    result = toLowerCase("");
    TEST_ASSERT(strcmp(result, "") == 0, "Empty string remains empty");
    free(result);
    
    result = toLowerCase("already lowercase");
    TEST_ASSERT(strcmp(result, "already lowercase") == 0, "Already lowercase unchanged");
    free(result);
    
    result = toLowerCase(NULL);
    TEST_ASSERT(result == NULL, "NULL input should return NULL");
}

void test_trim_whitespace() {
    printf("\n=== Testing trim_whitespace() ===\n");
    
    char test1[50] = "  hello  ";
    trim_whitespace(test1);
    TEST_ASSERT(strcmp(test1, "hello") == 0, "Trim leading and trailing spaces");
    
    char test2[50] = "no spaces";
    trim_whitespace(test2);
    TEST_ASSERT(strcmp(test2, "no spaces") == 0, "No spaces to trim");
    
    char test3[50] = "     ";
    trim_whitespace(test3);
    TEST_ASSERT(strcmp(test3, "") == 0, "Only spaces should become empty");
    
    char test4[50] = "\t\ntabs\n\t";
    trim_whitespace(test4);
    TEST_ASSERT(strcmp(test4, "tabs") == 0, "Trim tabs and newlines");
    
    char test5[50] = "  multiple   words  ";
    trim_whitespace(test5);
    TEST_ASSERT(strcmp(test5, "multiple   words") == 0, "Keep internal spaces");
}

void test_is_valid_date() {
    printf("\n=== Testing is_valid_date() ===\n");
    
    TEST_ASSERT(is_valid_date("2024-03-15") == 1, "Valid date: 2024-03-15");
    TEST_ASSERT(is_valid_date("2025-12-31") == 1, "Valid date: 2025-12-31");
    TEST_ASSERT(is_valid_date("2000-01-01") == 1, "Valid date: 2000-01-01");
    TEST_ASSERT(is_valid_date("2567-05-20") == 1, "Valid BE year: 2567-05-20");
    TEST_ASSERT(is_valid_date("2024-13-01") == 0, "Invalid month: 13");
    TEST_ASSERT(is_valid_date("2024-00-01") == 0, "Invalid month: 0");
    TEST_ASSERT(is_valid_date("2024-12-32") == 0, "Invalid day: 32");
    TEST_ASSERT(is_valid_date("2024-12-00") == 0, "Invalid day: 0");
    TEST_ASSERT(is_valid_date("20240315") == 0, "Missing dashes");
    TEST_ASSERT(is_valid_date("2024/03/15") == 0, "Wrong separator");
    TEST_ASSERT(is_valid_date("24-03-15") == 0, "Wrong year format");
    TEST_ASSERT(is_valid_date("2024-3-15") == 0, "Month not zero-padded");
    TEST_ASSERT(is_valid_date("") == 0, "Empty string");
    TEST_ASSERT(is_valid_date(NULL) == 0, "NULL pointer");
    TEST_ASSERT(is_valid_date("1800-01-01") == 0, "Year too old");
    TEST_ASSERT(is_valid_date("2200-01-01") == 0, "Year too far in future");
}

void test_parseScore() {
    printf("\n=== Testing parseScore() ===\n");
    
    TEST_ASSERT(parseScore("1") == 1, "Parse '1' -> 1");
    TEST_ASSERT(parseScore("3") == 3, "Parse '3' -> 3");
    TEST_ASSERT(parseScore("5") == 5, "Parse '5' -> 5");
    TEST_ASSERT(parseScore("0") == -1, "Invalid score 0 -> -1");
    TEST_ASSERT(parseScore("6") == -1, "Invalid score 6 -> -1");
    TEST_ASSERT(parseScore("-1") == -1, "Negative score -> -1");
    TEST_ASSERT(parseScore("abc") == -1, "Non-numeric -> -1");
    TEST_ASSERT(parseScore("3.5") == -1, "Decimal -> -1");
    TEST_ASSERT(parseScore("") == -1, "Empty string -> -1");
    TEST_ASSERT(parseScore(NULL) == -1, "NULL pointer -> -1");
    // Note: strtol() skips leading whitespace, so "  3" returns 3
    // This is expected behavior - we test that trailing text is rejected
    TEST_ASSERT(parseScore("3abc") == -1, "Trailing text -> -1");
    TEST_ASSERT(parseScore("3  ") == -1, "Trailing space -> -1");
}

void test_allocate_string() {
    printf("\n=== Testing allocate_string() ===\n");
    
    char *str1 = allocate_string("Hello");
    TEST_ASSERT(str1 != NULL, "Should allocate memory");
    TEST_ASSERT(strcmp(str1, "Hello") == 0, "Should copy string correctly");
    free(str1);
    
    char *str2 = allocate_string("");
    TEST_ASSERT(str2 != NULL, "Should allocate for empty string");
    TEST_ASSERT(strcmp(str2, "") == 0, "Empty string copied correctly");
    free(str2);
    
    char *str3 = allocate_string("This is a longer string with spaces!");
    TEST_ASSERT(str3 != NULL, "Should handle long strings");
    TEST_ASSERT(strcmp(str3, "This is a longer string with spaces!") == 0, "Long string copied correctly");
    free(str3);
    
    char *str4 = allocate_string(NULL);
    TEST_ASSERT(str4 == NULL, "NULL input should return NULL");
}

void test_string_edge_cases() {
    printf("\n=== Testing Edge Cases ===\n");
    
    // Test edit distance with special characters
    TEST_ASSERT(editDistance("test!", "test?") == 1, "Special chars: test! -> test? = 1 edit");
    TEST_ASSERT(editDistance("123", "456") == 3, "Numbers: 123 -> 456 = 3 edits");
    
    // Test toLowerCase with numbers and symbols
    char *result = toLowerCase("Test123!@#");
    TEST_ASSERT(strcmp(result, "test123!@#") == 0, "toLowerCase preserves numbers and symbols");
    free(result);
    
    // Test date with edge values
    TEST_ASSERT(is_valid_date("2024-02-29") == 1, "Leap year date: 2024-02-29");
    TEST_ASSERT(is_valid_date("2024-04-31") == 0, "April only has 30 days");
}

// ========== MAIN TEST RUNNER ==========

int main() {
    printf("\n");
    printf("╔════════════════════════════════════════════════╗\n");
    printf("║     CUSTOMER REVIEW SYSTEM - UNIT TESTS        ║\n");
    printf("╚════════════════════════════════════════════════╝\n");
    
    // Run all test suites
    test_min3();
    test_editDistance();
    test_toLowerCase();
    test_trim_whitespace();
    test_is_valid_date();
    test_parseScore();
    test_allocate_string();
    test_string_edge_cases();
    
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
        printf("🎉 ALL TESTS PASSED! 🎉\n");
        return 0;
    } else {
        printf("❌ SOME TESTS FAILED\n");
        return 1;
    }
}