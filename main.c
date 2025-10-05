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

typedef struct {
    int index;
    int distance;
    char matchType[20];
} SearchResult;

// Global variables for reviews
Review *reviews = NULL; // Dynamic array or reviews
int review_count = 0;
int capacity = 0;

// Global variables for undo delete
Review last_deleted_review;
int has_deleted = 0;  // 0 = no deletion to undo, 1 = can undo
int last_deleted_position = -1;  // Where it was in the array

// === function prototypes
void initialize_system();
void free_all_memory();
int load_reviews_from_csv(const char *filename);
int save_reviews_to_csv(const char *filename);
void createSampleCSV();
void add_review();
void display_all_reviews();
void update_review();
void display_full_review(int index);
void enhanced_search_menu();
void search_reviews();
SearchResult* searchWithTypoCorrection(const char* query, int* resultCount, int maxDistance);
void enhanced_delete_menu();
void delete_review_by_name();
void delete_by_selection();
void delete_all_by_user();
void delete_review_at_index(int index);
void show_statistics();
void display_search_results(int *found_indices, int count, const char *search_term);
void display_numbered_results(int *indices, int count);
char* allocate_string(const char *str);
void resize_review_array();
char* toLowerCase(const char *str);
void trim_whitespace(char *str);
int find_review_by_name(const char *name);
int find_partial_matches(const char *search_term, int *found_indices, int max_results);
int is_valid_date(const char *date_str);
int parseScore(const char* score_str);
int min3(int a, int b, int c);
int editDistance(const char* str1, const char* str2);
int backup_reviews(const char *backup_name);
int restore_from_backup(const char *filename);
void undo_last_delete();

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
        printf("\n╔════════════════════════════════════════╗\n");
        printf("║    Customer Review Management System  ║\n");
        printf("╚════════════════════════════════════════╝\n");
        printf("1. Add Review\n");
        printf("2. Display All Reviews\n");
        printf("3. Search Reviews\n");
        printf("4. Update Review\n");
        printf("5. Delete Review\n");
        printf("6. Statistics\n");
        printf("7. Backup/Restore\n");
        printf("8. Undo Last Delete\n");
        printf("9. Save & Exit\n");
        printf("Enter choice (1-9): ");

        if (scanf("%d", &choice) != 1) {
            int c;
            while ((c = getchar()) != '\n' && c != EOF);

            printf("Invalid input! Please enter a number.\n");
            choice = 0;
            continue;
        }
        getchar();

        switch(choice) {
            case 1:
                add_review();
                break;
            case 2:
                display_all_reviews();
                break;
            case 3:
                enhanced_search_menu();
                break;
            case 4:
                update_review();
                break;
            case 5:
                enhanced_delete_menu();
                break;
            case 6:
                show_statistics();
                break;
            case 7: {
                printf("\n1. Create Backup\n2. Restore Backup\nChoice: ");
                int backup_choice;
                scanf("%d", &backup_choice);
                getchar();
                if (backup_choice == 1) {
                    backup_reviews(NULL);
                } else if (backup_choice == 2) {
                    char filename[256];
                    printf("Enter backup filename: ");
                    fgets(filename, sizeof(filename), stdin);
                    filename[strcspn(filename, "\n")] = 0;
                    restore_from_backup(filename);
                }
                break;
            }
            case 8:
                undo_last_delete();
                break;
            case 9:
                save_reviews_to_csv("reviews.csv");
                printf("✅ Data saved successfully!\n");
                break;
            default:
                printf("❌ Invalid choice!\n");
        }
    } while (choice != 9);

    free_all_memory();
    printf("Bye\n");
    return 0;
}

// Function
// core system
void initialize_system() {
    capacity = 5;  // Start with capacity for 5 reviews
    reviews = (Review*)malloc(capacity * sizeof(Review));
    if (!reviews) {
        printf("Memory allocation failed!\n");
        exit(1);
    }
    review_count = 0;
}

void free_all_memory() {
    for (int i = 0; i < review_count; i++) {
        free(reviews[i].reviewer_name);
        free(reviews[i].review_date);
        free(reviews[i].feedback);
    }
    free(reviews);
    
    // Clean up undo memory if exists
    if (has_deleted) {
        free(last_deleted_review.reviewer_name);
        free(last_deleted_review.review_date);
        free(last_deleted_review.feedback);
    }
    
    printf("Memory cleaned up successfully\n");
}

// file I/O
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
    printf("Saved to reviews.csv\n");
    return 0;
}

// review operations
void add_review() {
    printf("\n=== Add New Review\n");

    if (review_count >= capacity) {
        resize_review_array();
    }

    char temp_name[256], temp_date[50], temp_feedback[512];
    int temp_score;

    printf("Enter reviewer name: ");
    fgets(temp_name, sizeof(temp_name), stdin);
    temp_name[strcspn(temp_name, "\n")] = 0;

    while (strlen(temp_name) == 0 || strlen(temp_name) > 50) {
        if (strlen(temp_name) == 0) {
            printf("Name cannot be empty! ");
        } else {
            printf("Name too long (max 50 characters)! ");
        }
        printf("Enter reviewer name: ");
        fgets(temp_name, sizeof(temp_name), stdin);
        temp_name[strcspn(temp_name, "\n")] = 0;
    }

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

    getchar();

    printf("Enter review date (YYYY-MM-DD): ");
    fgets(temp_date, sizeof(temp_date), stdin);
    temp_date[strcspn(temp_date, "\n")] = 0; // Remove newline

    while (!is_valid_date(temp_date)) {
        printf("Invalid date format! Use YYYY-MM-DD: ");
        fgets(temp_date, sizeof(temp_date), stdin);
        temp_date[strcspn(temp_date, "\n")] = 0;
    }

    printf("Enter feedback: ");
    fgets(temp_feedback, sizeof(temp_feedback), stdin);
    temp_feedback[strcspn(temp_feedback, "\n")] = 0;

    while (strlen(temp_feedback) > 200) {
        printf("Feedback too long! Max 200 characters. Try again: ");
        fgets(temp_feedback, sizeof(temp_feedback), stdin);
        temp_feedback[strcspn(temp_feedback, "\n")] = 0;
    }

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

    printf("\n=== All Customer Reviews ===\n");
    printf("%-4s %-20s %-6s %-12s %-50s\n", "#", "Reviewer", "Score", "Date", "Feedback");
    printf("------------------------------------------------------------------------\n");

    for (int i = 0; i < review_count; i++) {
        char display_feedback[51];
        if(strlen(reviews[i].feedback) > 50) {
            strncpy(display_feedback, reviews[i].feedback, 47);
            strcpy(display_feedback + 47, "...");
        } else {
            strcpy(display_feedback, reviews[i].feedback);
        }

        printf("%-4d %-20s %-6d %-12s %-50s\n",
                i + 1,
                reviews[i].reviewer_name,
                reviews[i].satisfaction_score,
                reviews[i].review_date,
                display_feedback);
    }

    printf("\nTotal reviews: %d\n", review_count);

    if (review_count > 0) {
        float total_score = 0;
        for (int i = 0; i < review_count; i++) {
            total_score += reviews[i].satisfaction_score;
        }
        printf("Average satisfaction score: %.2f/5\n", total_score / review_count);
    }
}

void update_review() {
    if (review_count == 0) {
        printf("No reviews to update.\n");
        return;
    }
    
    display_all_reviews();
    
    printf("\nEnter review number to update (1-%d): ", review_count);
    int choice;
    scanf("%d", &choice);
    getchar();
    
    if (choice < 1 || choice > review_count) {
        printf("Invalid choice!\n");
        return;
    }
    
    int index = choice - 1;
    
    printf("\n=== Updating Review #%d ===\n", choice);
    display_full_review(index);
    
    printf("\nWhat do you want to update?\n");
    printf("1. Reviewer Name\n");
    printf("2. Satisfaction Score\n");
    printf("3. Review Date\n");
    printf("4. Feedback\n");
    printf("5. Update All Fields\n");
    printf("Choice: ");
    
    int update_choice;
    scanf("%d", &update_choice);
    getchar();
    
    char temp_buffer[512];
    int temp_score;
    
    switch(update_choice) {
        case 1:
            printf("Enter new reviewer name: ");
            fgets(temp_buffer, sizeof(temp_buffer), stdin);
            temp_buffer[strcspn(temp_buffer, "\n")] = 0;
            free(reviews[index].reviewer_name);
            reviews[index].reviewer_name = allocate_string(temp_buffer);
            printf("✅ Name updated!\n");
            break;
            
        case 2:
            do {
                printf("Enter new satisfaction score (1-5): ");
                scanf("%d", &temp_score);
            } while (temp_score < 1 || temp_score > 5);
            reviews[index].satisfaction_score = temp_score;
            printf("✅ Score updated!\n");
            break;
            
        case 3:
            printf("Enter new review date (YYYY-MM-DD): ");
            fgets(temp_buffer, sizeof(temp_buffer), stdin);
            temp_buffer[strcspn(temp_buffer, "\n")] = 0;
            free(reviews[index].review_date);
            reviews[index].review_date = allocate_string(temp_buffer);
            printf("✅ Date updated!\n");
            break;
            
        case 4:
            printf("Enter new feedback: ");
            fgets(temp_buffer, sizeof(temp_buffer), stdin);
            temp_buffer[strcspn(temp_buffer, "\n")] = 0;
            free(reviews[index].feedback);
            reviews[index].feedback = allocate_string(temp_buffer);
            printf("✅ Feedback updated!\n");
            break;
            
        case 5:
            printf("Enter new reviewer name: ");
            fgets(temp_buffer, sizeof(temp_buffer), stdin);
            temp_buffer[strcspn(temp_buffer, "\n")] = 0;
            free(reviews[index].reviewer_name);
            reviews[index].reviewer_name = allocate_string(temp_buffer);
            
            do {
                printf("Enter new satisfaction score (1-5): ");
                scanf("%d", &temp_score);
                getchar();
            } while (temp_score < 1 || temp_score > 5);
            reviews[index].satisfaction_score = temp_score;
            
            printf("Enter new review date (YYYY-MM-DD): ");
            fgets(temp_buffer, sizeof(temp_buffer), stdin);
            temp_buffer[strcspn(temp_buffer, "\n")] = 0;
            free(reviews[index].review_date);
            reviews[index].review_date = allocate_string(temp_buffer);
            
            printf("Enter new feedback: ");
            fgets(temp_buffer, sizeof(temp_buffer), stdin);
            temp_buffer[strcspn(temp_buffer, "\n")] = 0;
            free(reviews[index].feedback);
            reviews[index].feedback = allocate_string(temp_buffer);
            
            printf("✅ All fields updated!\n");
            break;
            
        default:
            printf("Invalid choice!\n");
    }
}

void display_full_review(int index) {
    if (index < 0 || index >= review_count) {
        printf("Invalid review index!\n");
        return;
    }
    
    printf("\n╔════════════════════════════════════════╗\n");
    printf("║           Review Details               ║\n");
    printf("╚════════════════════════════════════════╝\n");
    printf("Reviewer:  %s\n", reviews[index].reviewer_name);
    printf("Score:     %d/5 ", reviews[index].satisfaction_score);
    for (int i = 0; i < reviews[index].satisfaction_score; i++) printf("⭐");
    printf("\n");
    printf("Date:      %s\n", reviews[index].review_date);
    printf("Feedback:  %s\n", reviews[index].feedback);
    printf("────────────────────────────────────────\n");
}

// search

void enhanced_search_menu() {
    printf("\n╔════════════════════════════════════════╗\n");
    printf("║         🔍 Search Options              ║\n");
    printf("╚════════════════════════════════════════╝\n");
    printf("1. Search by name (with typo correction)\n");
    printf("2. Search by score range\n");
    printf("3. Search by date\n");
    printf("4. Back to main menu\n");
    printf("Choice: ");
    
    int choice;
    scanf("%d", &choice);
    getchar();
    
    switch(choice) {
        case 1:
            search_reviews();
            break;
        case 2: {
            int min_score, max_score;
            printf("Enter minimum score (1-5): ");
            scanf("%d", &min_score);
            printf("Enter maximum score (1-5): ");
            scanf("%d", &max_score);
            getchar();
            
            printf("\n=== Reviews with score %d-%d ===\n", min_score, max_score);
            int found = 0;
            for (int i = 0; i < review_count; i++) {
                if (reviews[i].satisfaction_score >= min_score && 
                    reviews[i].satisfaction_score <= max_score) {
                    display_full_review(i);
                    found++;
                }
            }
            if (found == 0) printf("No reviews found in this range.\n");
            else printf("\nFound %d reviews.\n", found);
            break;
        }
        case 3: {
            char search_date[20];
            printf("Enter date (YYYY-MM-DD): ");
            fgets(search_date, sizeof(search_date), stdin);
            search_date[strcspn(search_date, "\n")] = 0;
            
            printf("\n=== Reviews on %s ===\n", search_date);
            int found = 0;
            for (int i = 0; i < review_count; i++) {
                if (strcmp(reviews[i].review_date, search_date) == 0) {
                    display_full_review(i);
                    found++;
                }
            }
            if (found == 0) printf("No reviews found on this date.\n");
            else printf("\nFound %d reviews.\n", found);
            break;
        }
        case 4:
            return;
        default:
            printf("Invalid choice!\n");
    }
}

void search_reviews() {
    char query[100];

    printf("\n╔════════════════════════════════════════╗\n");
    printf("║        🔍 Search Reviews               ║\n");
    printf("╚════════════════════════════════════════╝\n");
    printf("Enter reviewer name: ");
    fgets(query, sizeof(query), stdin);
    query[strcspn(query, "\n")] = 0;
    
    if (strlen(query) == 0) {
        printf("Search cancelled.\n");
        return;
    }
    
    printf("\n🔎 Searching for: '%s'\n", query);
    printf("────────────────────────────────────────\n");
    
    int resultCount;
    SearchResult* results = searchWithTypoCorrection(query, &resultCount, 3);
    
    if (resultCount == 0) {
        printf("❌ No matches found.\n");
        printf("\n💡 Tips:\n");
        printf("  • Check spelling\n");
        printf("  • Try shorter search term\n");
        printf("  • Use partial name\n");
        free(results);
        return;
    }
    
    int exactCount = 0, closeCount = 0, fuzzyCount = 0;
    
    for (int i = 0; i < resultCount; i++) {
        if (strcmp(results[i].matchType, "exact") == 0) exactCount++;
        else if (strcmp(results[i].matchType, "close") == 0) closeCount++;
        else fuzzyCount++;
    }
    
    // Display exact matches
    if (exactCount > 0) {
        printf("\n✅ Exact Matches (%d):\n", exactCount);
        for (int i = 0; i < resultCount; i++) {
            if (strcmp(results[i].matchType, "exact") == 0) {
                int idx = results[i].index;
                printf("  %d. %s (Score: %d/5, Date: %s)\n",
                       idx + 1,
                       reviews[idx].reviewer_name,
                       reviews[idx].satisfaction_score,
                       reviews[idx].review_date);
                printf("     💬 %s\n", reviews[idx].feedback);
            }
        }
    }
    
    // Display close matches
    if (closeCount > 0) {
        printf("\n🔍 Close Matches (%d) - Did you mean?\n", closeCount);
        for (int i = 0; i < resultCount; i++) {
            if (strcmp(results[i].matchType, "close") == 0) {
                int idx = results[i].index;
                printf("  %d. %s (Edit distance: %d)\n",
                       idx + 1,
                       reviews[idx].reviewer_name,
                       results[i].distance);
                printf("     Score: %d/5 | Date: %s\n",
                       reviews[idx].satisfaction_score,
                       reviews[idx].review_date);
            }
        }
    }
    
    // Display fuzzy matches
    if (fuzzyCount > 0) {
        printf("\n💡 Similar Names (%d):\n", fuzzyCount);
        for (int i = 0; i < resultCount; i++) {
            if (strcmp(results[i].matchType, "fuzzy") == 0) {
                int idx = results[i].index;
                printf("  %d. %s (Distance: %d)\n",
                       idx + 1,
                       reviews[idx].reviewer_name,
                       results[i].distance);
            }
        }
    }
    
    free(results);
}

SearchResult* searchWithTypoCorrection(const char* query, int* resultCount, int maxDistance) {
    if (!query || review_count == 0) {
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
    for (int i = 0; i < review_count; i++) {
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

// delete

void enhanced_delete_menu() {
    printf("\n╔════════════════════════════════════════╗\n");
    printf("║         🗑️  Delete Options             ║\n");
    printf("╚════════════════════════════════════════╝\n");
    printf("1. Delete by name\n");
    printf("2. Delete by selecting from list\n");
    printf("3. Delete all reviews by a user\n");
    printf("4. Back to main menu\n");
    printf("Choice: ");
    
    int choice;
    scanf("%d", &choice);
    getchar();
    
    switch(choice) {
        case 1:
            delete_review_by_name();
            break;
        case 2:
            delete_by_selection();
            break;
        case 3:
            delete_all_by_user();
            break;
        case 4:
            return;
        default:
            printf("Invalid choice!\n");
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
    fgets(search_name, sizeof(search_name), stdin);
    search_name[strcspn(search_name, "\n")] = 0;

    // Use typo correction to find matches
    int resultCount;
    SearchResult* results = searchWithTypoCorrection(search_name, &resultCount, 3);
    
    if (resultCount == 0) {
        printf("Review not found!\n");
        free(results);
        return;
    }
    
    // If multiple matches, show them
    if (resultCount > 1) {
        printf("\nMultiple matches found:\n");
        for (int i = 0; i < resultCount; i++) {
            int idx = results[i].index;
            printf("%d. %s (Score: %d/5, Date: %s)\n",
                   i + 1,
                   reviews[idx].reviewer_name,
                   reviews[idx].satisfaction_score,
                   reviews[idx].review_date);
        }
        printf("\nSelect review to delete (1-%d), or 0 to cancel: ", resultCount);
        int choice;
        scanf("%d", &choice);
        getchar();
        
        if (choice < 1 || choice > resultCount) {
            printf("Delete cancelled.\n");
            free(results);
            return;
        }
        
        delete_review_at_index(results[choice - 1].index);
    } else {
        // Single match - confirm and delete
        int idx = results[0].index;
        printf("Found review:\n");
        printf("Reviewer: %s\n", reviews[idx].reviewer_name);
        printf("Score: %d/5\n", reviews[idx].satisfaction_score);
        printf("Date: %s\n", reviews[idx].review_date);
        printf("Feedback: %s\n", reviews[idx].feedback);
        
        char confirm;
        printf("Are you sure you want to delete this review? (y/n): ");
        scanf(" %c", &confirm);
        getchar();
        
        if (confirm == 'y' || confirm == 'Y') {
            delete_review_at_index(idx);
        } else {
            printf("Deletion cancelled.\n");
        }
    }
    
    free(results);
}

void delete_by_selection() {
    if (review_count == 0) {
        printf("No reviews to delete.\n");
        return;
    }
    
    display_all_reviews();
    
    printf("\nEnter review number to delete (1-%d): ", review_count);
    int choice;
    scanf("%d", &choice);
    getchar();
    
    if (choice < 1 || choice > review_count) {
        printf("Invalid choice!\n");
        return;
    }
    
    delete_review_at_index(choice - 1);
}

void delete_all_by_user() {
    if (review_count == 0) {
        printf("No reviews to delete.\n");
        return;
    }
    
    char search_name[256];
    printf("Enter reviewer name: ");
    fgets(search_name, sizeof(search_name), stdin);
    search_name[strcspn(search_name, "\n")] = 0;
    
    int deleted_count = 0;
    for (int i = review_count - 1; i >= 0; i--) {
        if (strcmp(reviews[i].reviewer_name, search_name) == 0) {
            free(reviews[i].reviewer_name);
            free(reviews[i].review_date);
            free(reviews[i].feedback);
            
            for (int j = i; j < review_count - 1; j++) {
                reviews[j] = reviews[j + 1];
            }
            review_count--;
            deleted_count++;
        }
    }
    
    if (deleted_count > 0) {
        printf("✅ Deleted %d review(s) by %s\n", deleted_count, search_name);
    } else {
        printf("❌ No reviews found for %s\n", search_name);
    }
}

void delete_review_at_index(int index) {
    if (index < 0 || index >= review_count) {
        printf("Invalid index!\n");
        return;
    }
    
    printf("\n⚠️  You are about to delete:\n");
    display_full_review(index);
    
    printf("\n❗ Type 'DELETE' to confirm: ");
    char confirm[20];
    scanf("%s", confirm);
    getchar();
    
    if (strcmp(confirm, "DELETE") == 0) {
        // Save for undo (make copies before freeing!)
        if (has_deleted) {
            // Free previous undo if exists
            free(last_deleted_review.reviewer_name);
            free(last_deleted_review.review_date);
            free(last_deleted_review.feedback);
        }
        
        // store this thing for undo later on
        last_deleted_review.reviewer_name = allocate_string(reviews[index].reviewer_name);
        last_deleted_review.satisfaction_score = reviews[index].satisfaction_score;
        last_deleted_review.review_date = allocate_string(reviews[index].review_date);
        last_deleted_review.feedback = allocate_string(reviews[index].feedback);
        last_deleted_position = index;
        has_deleted = 1;
        
        // Now delete
        free(reviews[index].reviewer_name);
        free(reviews[index].review_date);
        free(reviews[index].feedback);
        
        for (int i = index; i < review_count - 1; i++) {
            reviews[i] = reviews[i + 1];
        }
        
        review_count--;
        printf("✅ Review deleted!\n");
        printf("💡 Tip: Use menu option 9 to undo if this was a mistake.\n");
    } else {
        printf("❌ Deletion cancelled.\n");
    }
}

void undo_last_delete() {
    if (!has_deleted) {
        printf("❌ No deletion to undo.\n");
        return;
    }
    
    printf("\n♻️  Restoring deleted review:\n");
    printf("  Name: %s\n", last_deleted_review.reviewer_name);
    printf("  Score: %d/5\n", last_deleted_review.satisfaction_score);
    printf("  Date: %s\n", last_deleted_review.review_date);
    
    // Check capacity
    if (review_count >= capacity) {
        resize_review_array();
    }
    
    // Insert at original position or end if position invalid
    int insert_pos = last_deleted_position;
    if (insert_pos < 0 || insert_pos > review_count) {
        insert_pos = review_count;
    }
    
    // Shift elements to make room
    for (int i = review_count; i > insert_pos; i--) {
        reviews[i] = reviews[i - 1];
    }
    
    // Restore the review
    reviews[insert_pos].reviewer_name = last_deleted_review.reviewer_name;
    reviews[insert_pos].satisfaction_score = last_deleted_review.satisfaction_score;
    reviews[insert_pos].review_date = last_deleted_review.review_date;
    reviews[insert_pos].feedback = last_deleted_review.feedback;
    
    review_count++;
    has_deleted = 0;  // Can only undo once
    
    printf("✅ Review restored successfully!\n");
}

// statics and display

void show_statistics() {
    if (review_count == 0) {
        printf("\n📊 No data to show statistics.\n");
        return;
    }
    
    printf("\n╔════════════════════════════════════════╗\n");
    printf("║           📊 Statistics                ║\n");
    printf("╚════════════════════════════════════════╝\n");
    
    int score_counts[5] = {0};
    float total = 0;
    
    for (int i = 0; i < review_count; i++) {
        int score = reviews[i].satisfaction_score;
        if (score >= 1 && score <= 5) {
            score_counts[score - 1]++;
            total += score;
        }
    }
    
    printf("Total Reviews: %d\n", review_count);
    printf("Average Score: %.2f/5\n\n", total / review_count);
    
    printf("Score Distribution:\n");
    for (int i = 4; i >= 0; i--) {
        printf("%d ⭐ ", i + 1);
        int bars = (score_counts[i] * 20) / review_count;
        for (int j = 0; j < bars; j++) printf("█");
        printf(" (%d)\n", score_counts[i]);
    }
}

void display_search_results(int *found_indices, int count, const char *search_term);
void display_numbered_results(int *indices, int count);

// helper functions

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

void resize_review_array() {
    capacity *= 2;
    reviews = (Review*)realloc(reviews, capacity * sizeof(Review));
    if (!reviews) {
        printf("Memory reallocation failed!!\n");
        exit(1);
    }
    printf("Array resized to capacity: %d\n", capacity);
}

char* toLowerCase(const char *str) {
    if (!str) return NULL;

    char* lower = malloc(strlen(str) +1 );
    if (!lower) return NULL; // only check malloc result

    for (int i = 0; str[i]; i++) {
        lower[i] = tolower(str[i]);
    }
    lower[strlen(str)] = '\0';
    return lower;
}

void trim_whitespace(char *str) {
    if (!str) return;
    
    // Trim leading spaces
    char *start = str;
    while (*start && isspace(*start)) start++;
    
    // Trim trailing spaces
    char *end = start + strlen(start) - 1;
    while (end > start && isspace(*end)) end--;
    
    // Copy trimmed string
    size_t len = end - start + 1;
    memmove(str, start, len);
    str[len] = '\0';
}

int find_review_by_name(const char *name) {
    for (int i = 0; i < review_count; i++) {
        if (strcmp(reviews[i].reviewer_name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int find_partial_matches(const char *search_term, int *found_indices, int max_results);

// validation

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
    
    // Check year is in valid range (BE or CE) - DON'T RETURN YET
    int valid_year = 0;
    if (year >= 2400 && year <= 2700) {
        valid_year = 1; // BE year
    } else if (year >= 1900 && year <= 2100) {
        valid_year = 1; // CE year
    }
    
    if (!valid_year) return 0;  // Invalid year range
    
    // NOW check month and day (this code will actually run now!)
    if (month < 1 || month > 12) return 0;
    if (day < 1 || day > 31) return 0;
    
    int days_in_month[] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (day > days_in_month[month - 1]) return 0;
    
    return 1;  // All checks passed
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

// typo matching

int min3(int a, int b, int c) {
    int min = a;
    if (b < min) min = b;
    if (c < min) min = c;
    return min;
}

/**
 * Calculate Levenshtein Distance (Edit Distance) using "Levenshtein distance algorithm"
 * Returns number of edits needed to transform str1 to str2
 * editDistance("kitten", "sitting") = 3
 *   editDistance("john", "jhon") = 2
 *   editDistance("sarah", "sara") = 1
 * use malloc instead of VLA for better memory safety
 */

int editDistance(const char* str1, const char* str2) {
    if (!str1 || !str2) return 999;

    int len1 = strlen(str1);
    int len2 = strlen(str2);
    
    // FIX: Use len1, not len
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
            return 999;  // FIX: Add semicolon and return
        }
    }
    
    // Initialize first column
    for (int i = 0; i <= len1; i++) {
        dp[i][0] = i;
    }

    // Initialize first row
    for (int j = 0; j <= len2; j++) {
        dp[0][j] = j;
    }

    // Fill the DP table - FIX: Complete the assignment
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

// backup/restore

int backup_reviews(const char *backup_name) {
    char filename[256];
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    
    if (backup_name) {
        snprintf(filename, sizeof(filename), "backup_%s.csv", backup_name);
    } else {
        snprintf(filename, sizeof(filename), 
                 "backup_%04d%02d%02d_%02d%02d%02d.csv",
                 t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
                 t->tm_hour, t->tm_min, t->tm_sec);
    }
    
    if (save_reviews_to_csv(filename) == 0) {
        printf("✅ Backup created: %s\n", filename);
        return 0;
    }
    return -1;
}

int restore_from_backup(const char *filename) {
    printf("⚠️  This will replace current data!\n");
    printf("Continue? (y/n): ");
    char confirm;
    scanf(" %c", &confirm);
    getchar();
    
    if (confirm != 'y' && confirm != 'Y') {
        printf("Restore cancelled.\n");
        return -1;
    }
    
    // Free current data
    free_all_memory();
    initialize_system();
    
    if (load_reviews_from_csv(filename) == 0) {
        printf("✅ Data restored from: %s\n", filename);
        return 0;
    } else {
        printf("❌ Failed to restore backup!\n");
        return -1;
    }
}