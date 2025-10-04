#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

// Structure for user data
typedef struct {
    char *reviewer_name;
    int satisfaction_score;
    char *review_date;
    char *feedback;
} Review;

Review *reviews = NULL; // Dynamic array or reviews
int review_count = 0;
int capacity = 0;

// Core operations with malloc
void initialize_system();
int load_reviews_from_csv(const char *filename);
int save_reviews_to_csv(const char *filename);
void add_review();
void display_all_reviews();
void enchanced_search_menu();
void enchanced_delete_menu();
void free_all_memory();

// Search and display functions
void search_reviews_partial();
void display_search_results(int *found_indices, int count, const char *search_term);
void display_full_review(int index);
void show_statistics();

// Delete functions
void delete_review_by_name();
void delete_by_selection();
void delete_all_by_user();
void delete_review_at_index(int index);

// Input validation functions
int get_safe_int(const char *prompt, int min_val, int max_val);
char* get_safe_string(const char *prompt, int max_length);
char* get_safe_date(const char *prompt);
int is_valid_date(const char *date);

// Helper functions
char* allocate_string(const char *str);
void resize_review_array();
int find_partial_matches(const char *search_term, int *found_indices, int max_results);



void trim_whitespace(char *str);
void display_numbered_results(int *indices, int count);

int main() {
    printf("=== Customer Review Management System ===\n");

    initialize_system();

    if (load_reviews_from_csv("reviews.csv") == 0) {
        printf("Loaded existing review data\n");
    } else {
        printf("Starting with nothing\n");
    }

    int choice;
    do {
        printf("\n=== Main Menu ===\n");
        printf("1. Add Review\n");
        printf("2. Display All Reviews\n");
        printf("3. Delete Review\n");
        printf("4. Save & Exit\n");
        printf("Enter choice (1-4): ");

        scanf("%d", &choice);

        switch(choice) {
            case 1:
                add_review();
                break;
            case 2:
                display_all_reviews();
                break;
            case 3:
                delete_review_by_name();
                break;
            case 4:
                save_reviews_to_csv("reviews.csv");
                printf("✅ Data saved successfully!\n");
                break;
            default:
                printf("❌ Invalid choice!\n");
        }
    } while (choice != 4);

    free_all_memory();
    printf("Bye\n");
    return 0;
}

// Initialize the system
void initialize_system() {
    capacity = 5;  // Start with capacity for 5 reviews
    reviews = (Review*)malloc(capacity * sizeof(Review));
    if (!reviews) {
        printf("Memory allocation failed!\n");
        exit(1);
    }
    review_count = 0;
}

int load_reviews_from_csv(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        return -1;
    }

    char line[1024]; // Prepare to collect header and never use it again

    // Skip header line
    if (fgets(line, sizeof(line), file) == NULL) { // read one line, put it in the variable named "line"
        fclose(file);
        return -1;
    }

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;

        // Parse csv line and handle commas in feedback
        char *reviewer_name = strtok(line, ",");
        char *score_str = strtok(NULL, ",");
        char *review_date = strtok(NULL, ",");
        char *feedback = strtok(NULL, ""); // Get the rest, No more commas errors or syntax errors
        if (reviewer_name && score_str && review_date && feedback) {
            if (review_count >= capacity) {
                resize_review_array();
            }

            // Remove leading space from feedback
            while (*feedback == ' ') feedback++;

            // Allocate memory and copy data (This part is complex)
            // Array_name[index]
            reviews[review_count].reviewer_name = allocate_string(reviewer_name);
            reviews[review_count].satisfaction_score = atoi(score_str);
            reviews[review_count].review_date = allocate_string(review_date);
            reviews[review_count].feedback = allocate_string(feedback);
            review_count++;
        }
    }

    fclose(file);
    return 0;
}

int save_reviews_to_csv(const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        printf("Cannot create/open file for writing!\n");
        return -1;
    }
    
    // Write header
    fprintf(file, "ReviewerName,SatisfactionScore,ReviewDate,Feedback\n");
    
    // Write review data
    for (int i = 0; i < review_count; i++) {
        fprintf(file, "%s,%d,%s,%s\n", 
                reviews[i].reviewer_name, 
                reviews[i].satisfaction_score,
                reviews[i].review_date,
                reviews[i].feedback);
    }
    
    fclose(file);
    return 0;
}

void add_review() {
    printf("\n=== Add New Review\n");

    if (review_count >= capacity) {
        resize_review_array();
    }

    char temp_name[256], temp_date[50], temp_feedback[512];
    int temp_score;

    printf("Enter reviewer name: ");
    getchar(); // Clear input buffer
    fgets(temp_name, sizeof(temp_name), stdin);
    temp_name[strcspn(temp_name, "\n")] = 0;

    do {
        printf("Enter satisfaction score (1-5): ");
        int result = scanf("%d", &temp_score);
        if (result != 1) {
            printf("Invalid input! Enter number instead\n");

            int c;
            while ((c = getchar()) != '\n' && c != EOF);

            temp_score = 0;
        } else if (temp_score < 1 || temp_score > 5) {
            printf("Please enter a score between 1 to 5\n");
        }
    } while (temp_score < 1 || temp_score > 5);

    printf("Enter review date (YYYY-MM-DD): ");
    getchar();
    fgets(temp_date, sizeof(temp_date), stdin);
    temp_date[strcspn(temp_date, "\n")] = 0; // Remove newline

    printf("Enter feedback: ");
    fgets(temp_feedback, sizeof(temp_feedback), stdin);
    temp_feedback[strcspn(temp_feedback, "\n")] = 0;

    reviews[review_count].reviewer_name = allocate_string(temp_name);
    reviews[review_count].satisfaction_score = temp_score;
    reviews[review_count].review_date = allocate_string(temp_date);
    reviews[review_count].feedback = allocate_string(temp_feedback);
    review_count++;
    printf("Review added!! yay\n");
}

void display_all_reviews() {
    printf("\n=== All Customer Reviews ===\n");

    if (review_count == 0) {
        printf("No reviews found.\n");
        return;
    }

    printf("%-20s %-6s %-12s %-50s\n", "Reviewer", "Score", "Date", "Feedback");
    printf("%-20s %-6s %-12s %-50s\n", "--------", "-----", "----", "--------");

    for (int i = 0; i < review_count; i++) {
        // Truncate feedback if too long for display, prob doesnt apply to name because it ruin the search function
        char display_feedback[51];
        strncpy(display_feedback, reviews[i].feedback, 50);
        display_feedback[47] = '\0';
        if (strlen(reviews[i].feedback) > 50) {
            strcat(display_feedback, "...");
        } else {
            strcpy(display_feedback, reviews[i].feedback);
        }

        // Now we can print it out
        printf("%-20s %-6d %-12s %-50s\n",
                reviews[i].reviewer_name,
                reviews[i].satisfaction_score,
                reviews[i].review_date,
                display_feedback);
    }

    printf("\nTotal reviews: %d\n", review_count);

    // calculate average score
    if (review_count > 0) {
        float total_score = 0;
        for (int i = 0; i < review_count; i++) {
            total_score += reviews[i].satisfaction_score;
        }
        printf("Average satisfaction score: %.2f/5\n", total_score / review_count);
    }
}

void delete_review_by_name() {
    if (review_count == 0) {
        printf("No reviews to delete.\n");
        return;
    }

    char search_name[256];
    printf("\n=== Delete Review ===\n");
    printf("Enter reviewer name to delete: ");
    getchar();
    fgets(search_name, sizeof(search_name), stdin);
    search_name[strcspn(search_name, "\n")] = 0;

    // find actual data in form of list number
    int index = find_review_by_name(search_name);

    if (index == -1) {
        printf("Review not found!\n");
        return;
    }

    printf("Found review:\n");
    printf("Reviewer: %s\n", reviews[index].reviewer_name);
    printf("Score: %d/5\n", reviews[index].satisfaction_score);
    printf("Date: %s\n", reviews[index].review_date);
    printf("Feedback: %s\n", reviews[index].feedback);

    char confirm;
    printf("Are you sure you want to delete this review? (y/n): ");
    scanf(" %c", &confirm);

    // actually free memory for the review being deleted
    if (confirm == 'y' || confirm == 'Y') {
        free(reviews[index].reviewer_name);
        free(reviews[index].review_date);
        free(reviews[index].feedback);

        // shift remaining reviews down
        for (int i = index; i < review_count - 1; i++) {
            reviews[i] = reviews[i + 1];
        }

        review_count--;
        printf("Review deleted successfully\n");
    } else {
        printf("Deletion cancelled.\n");
    }
}

char* allocate_string(const char *str) {
    if (!str) return NULL; // check if it's an empty pointer

    char *new_str = (char*)malloc(strlen(str) + 1);
    if (!new_str) {
        printf("Memory allocation failed for string\n");
        exit(1); // netter to terminate cleanly than crash mysteriously
    }
    strcpy(new_str, str); // not just copies the pointer, but the actual characters to the new_str with malloc pointer
    return new_str;
}

// will make a default capacity size later on, 0 * 2 is always 0 anyways
void resize_review_array() {
    capacity *= 2;
    reviews = (Review*)realloc(reviews, capacity * sizeof(Review));
    if (!reviews) {
        printf("Memory reallocation failed!!\n");
        exit(1);
    }
    printf("Array resized to capacity: %d\n", capacity);
}

int find_review_by_name(const char *name) {
    for (int i = 0; i < review_count; i++) {
        if (strcmp(reviews[i].reviewer_name, name) == 0) {
            return i;
        }
    }
    return -1;
}

void free_all_memory() {
    for (int i = 0; i < review_count; i++) {
        free(reviews[i].reviewer_name);
        free(reviews[i].review_date);
        free(reviews[i].feedback);
    }
    free(reviews);
    printf("Memory cleaned up successfully\n");
}

// === typo matching ===

// function for typo matching
char* toLowerCase(const char *str) {
    if (!str) return NULL;

    char* lower = malloc(strlen(str) +1 );
    if (!tolower) return NULL;

    for (int i = 0; str[i]; i++) {
        lower[i] = tolower(str[i]);
    }
    lower[strlen(str)] = '\0';
    return lower;
}

// function for typo matching
int min3(int a, int b, int c) {
    int min = a;
    if (b < min) min = b;
    if (c < min) min = c;
    return min;
}

/**
 * Calculate Levenshtein Distance (Edit Distance)
 * Returns number of edits needed to transform str1 to str2
 * editDistance("kitten", "sitting") = 3
 *   editDistance("john", "jhon") = 2
 *   editDistance("sarah", "sara") = 1
 */

int editDistance(const char* str1, const char* str2) {
    if (!str1 || !str2) return 999;

    int len1 = strlen(str1);
    int len2 = strlen(str2);

    int dp[len1 + 1][len2 + 1];
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
    return dp[len1][len2];
}

typedef struct {
    int index;
    int distance;
    char matchType[20];
} SearchResult;

SearchResult* searchWithTypoCorrection(const char* query, int* resultCount, int maxDistance) {
    if (!query || review\_count == 0) {
        *resultCount = 0;
        return NULL;
    }

    // allocate results array
    SearchResult* results = malloc(review_count * sizeof(SearchResult));
    if (!results) {
        *resultCount = 0;
        return NULL;
    }

    *resultCount = 0;
    char* lowerQuery = toLowerCase(query);
    
    // find match
    for (int i = 0; i < reviewCount; i++) {
        char* lowerName = toLowerCase(reviews[i].reviewer_name);
        // calculate edit distance
        int distance = editDistance(lowerQuery, lowerName);

        if (distance <= maxDistance) {
            results[*resultCount].index = i;
            results[*resultCount].distance = distance;

            if (distance == 0) {
                strcpy(results[*resultCount].matchType, "exact");
            } else if (distance == 2) {
                strcpy(results[*resultCount].matchType, "close");
            } else {
                strcpy(results[*resultCount].matchType, "fuzzy");
            }
            (*resultCount)++;
        }
        free(lowerName);
    }
    free(lowerQuery);

    // result sorted by distance from (best matches first)
    for (int i = 0; i < *resultCount - 1; i++) {
        for (int j = 0; j < *resultCount - i - 1; j++) {
            if (results[j].distance > results[j+1].distance) {
                // structType VariableName = array[x] ; in this case "temp" is auto clear, just like int i = 5
                SearchResult temp = results[j];
                results[j] = results[j+1];
                results[j+1] = temp;
            }
        }
    }
    return results;
}