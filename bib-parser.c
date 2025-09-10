#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE_LENGTH 1024
#define MAX_AUTHORS 100
#define MAX_AUTHOR_LENGTH 256
#define MAX_INSTITUTE_LENGTH 128

// Structure to hold author information
typedef struct {
    char name[MAX_AUTHOR_LENGTH];
    char affiliation[MAX_AUTHOR_LENGTH];
} Author;

// Function to trim whitespace from a string
char* trim(char* str) {
    char* end;

    // Trim leading space
    while(isspace((unsigned char)*str)) str++;

    if(*str == 0) return str;

    // Trim trailing space
    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;

    end[1] = '\0';
    return str;
}

// Function to convert string to lowercase
void to_lowercase(char* str) {
    for(int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
}

// Function to extract authors from a BibTeX author field
int extract_authors(char* author_field, Author authors[], int max_authors) {
    int author_count = 0;
    char* token;
    char* author_copy = strdup(author_field);

    // Remove braces if present
    if (author_copy[0] == '{') {
        memmove(author_copy, author_copy + 1, strlen(author_copy));
        if (author_copy[strlen(author_copy) - 1] == '}') {
            author_copy[strlen(author_copy) - 1] = '\0';
        }
    }

    // Split by "and" to separate authors
    token = strtok(author_copy, " and ");
    while (token != NULL && author_count < max_authors) {
        strncpy(authors[author_count].name, trim(token), MAX_AUTHOR_LENGTH - 1);
        authors[author_count].name[MAX_AUTHOR_LENGTH - 1] = '\0';
        authors[author_count].affiliation[0] = '\0'; // Initialize empty
        author_count++;
        token = strtok(NULL, " and ");
    }

    free(author_copy);
    return author_count;
}

// Function to check if an author is from the specified institute
int is_from_institute(const char* author_name, const char* institute_name) {
    char author_lower[MAX_AUTHOR_LENGTH];
    char institute_lower[MAX_INSTITUTE_LENGTH];

    strncpy(author_lower, author_name, MAX_AUTHOR_LENGTH - 1);
    strncpy(institute_lower, institute_name, MAX_INSTITUTE_LENGTH - 1);

    to_lowercase(author_lower);
    to_lowercase(institute_lower);

    // Simple check - in a real implementation, you might want to:
    // 1. Check against a list of known institute authors
    // 2. Look for email domains
    // 3. Check affiliation fields if present in the BibTeX

    // For now, we'll check if the institute name appears in the author field
    // This is a simplified approach - you may need to customize this logic
    return strstr(author_lower, institute_lower) != NULL;
}

// Function to parse BibTeX file and count institute authors
int parse_bib_file(const char* filename, const char* institute_name) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error: Cannot open file %s\n", filename);
        return -1;
    }

    char line[MAX_LINE_LENGTH];
    int total_institute_authors = 0;
    int total_entries = 0;
    int in_entry = 0;
    Author authors[MAX_AUTHORS];

    printf("Parsing BibTeX file: %s\n", filename);
    printf("Looking for authors from: %s\n\n", institute_name);

    while (fgets(line, sizeof(line), file)) {
        char* trimmed_line = trim(line);

        // Check if we're starting a new entry
        if (trimmed_line[0] == '@') {
            in_entry = 1;
            total_entries++;
            continue;
        }

        // Skip if we're not in an entry
        if (!in_entry) continue;

        // Check for end of entry
        if (strchr(trimmed_line, '}') && strlen(trimmed_line) == 1) {
            in_entry = 0;
            continue;
        }

        // Look for author field
        if (strncmp(trimmed_line, "author", 6) == 0) {
            char* equals_pos = strchr(trimmed_line, '=');
            if (equals_pos) {
                char* author_field = equals_pos + 1;
                author_field = trim(author_field);

                // Remove trailing comma if present
                int len = strlen(author_field);
                if (len > 0 && author_field[len-1] == ',') {
                    author_field[len-1] = '\0';
                }

                // Extract authors
                int author_count = extract_authors(author_field, authors, MAX_AUTHORS);

                // Count institute authors
                int institute_authors_in_entry = 0;
                for (int i = 0; i < author_count; i++) {
                    if (is_from_institute(authors[i].name, institute_name)) {
                        institute_authors_in_entry++;
                        printf("Found institute author: %s\n", authors[i].name);
                    }
                }

                if (institute_authors_in_entry > 0) {
                    printf("Entry %d has %d author(s) from %s\n",
                           total_entries, institute_authors_in_entry, institute_name);
                    total_institute_authors += institute_authors_in_entry;
                }
            }
        }
    }

    fclose(file);

    printf("\n=== Summary ===\n");
    printf("Total BibTeX entries processed: %d\n", total_entries);
    printf("Total authors from %s: %d\n", institute_name, total_institute_authors);

    return total_institute_authors;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: %s <bib_file> <institute_name>\n", argv[0]);
        printf("Example: %s papers.bib \"MIT\"\n", argv[0]);
        printf("Example: %s papers.bib \"University of California\"\n", argv[0]);
        return 1;
    }

    const char* bib_file = argv[1];
    const char* institute_name = argv[2];

    int result = parse_bib_file(bib_file, institute_name);

    if (result >= 0) {
        printf("\nProgram completed successfully.\n");
        return 0;
    } else {
        printf("\nProgram failed with errors.\n");
        return 1;
    }
}
