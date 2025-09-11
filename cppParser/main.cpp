// main.cpp - Main program implementation
#include "bibdatabase.h"

extern "C" {
    int printf(const char* format, ...);
}

// Function prototypes
void print_usage(const char* program_name);
bool validate_arguments(int argc, char* argv[]);
void demonstrate_sorting(BibDatabase& db);
void demonstrate_merging();

int main(int argc, char* argv[]) {
    // Validate command line arguments
    if (!validate_arguments(argc, argv)) {
        print_usage(argv[0]);
        return 1;
    }

    const char* bib_file = argv[1];
    const char* institute_name = argv[2];

    // Create database and load from file
    BibDatabase database("Main Bibliography Database");

    printf("=== BibTeX Parser (C++ Version) ===\n");
    printf("Using OOP principles without standard libraries\n\n");

    // Load the bibliography file
    MyString filename(bib_file);
    if (!database.load_from_file(filename)) {
        printf("Failed to load bibliography file: %s\n", bib_file);
        return 1;
    }

    // Display database summary
    database.print_summary();

    // Count and display institute authors
    MyString institute(institute_name);
    printf("=== Institute Author Analysis ===\n");
    database.print_institute_authors(institute);

    // Demonstrate sorting (requirement 2)
    printf("\n=== Sorting Demonstration ===\n");
    demonstrate_sorting(database);

    // Demonstrate merging (requirement 3)
    printf("\n=== Merging Demonstration ===\n");
    demonstrate_merging();

    printf("\nProgram completed successfully.\n");
    return 0;
}

void print_usage(const char* program_name) {
    printf("Usage: %s <bib_file> <institute_name>\n", program_name);
    printf("\n");
    printf("Examples:\n");
    printf("  %s papers.bib \"IIIT\"\n", program_name);
    printf("  %s references.bib \"University of California\"\n", program_name);
    printf("\n");
    printf("This program:\n");
    printf("1. Parses BibTeX files using C++ OOP principles\n");
    printf("2. Sorts entries by <year descending, title ascending>\n");
    printf("3. Supports merging multiple bibliography databases\n");
    printf("4. Does not use any standard C/C++ libraries\n");
}

bool validate_arguments(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Error: Incorrect number of arguments\n");
        return false;
    }

    if (!argv[1] || MyString::strlen(argv[1]) == 0) {
        printf("Error: Empty bibliography filename\n");
        return false;
    }

    if (!argv[2] || MyString::strlen(argv[2]) == 0) {
        printf("Error: Empty institute name\n");
        return false;
    }

    return true;
}

void demonstrate_sorting(BibDatabase& db) {
    printf("Sorting entries by <year descending, title ascending>...\n");

    // Show entries before sorting
    printf("\nBefore sorting (first 5 entries):\n");
    unsigned long max_show = db.size() < 5 ? db.size() : 5;
    for (unsigned long i = 0; i < max_show; i++) {
        const BibEntry& entry = db.get_entry(i);
        printf("%lu. [%s] %s\n", i+1, 
               entry.get_year().c_str(), 
               entry.get_title().c_str());
    }

    // Sort the database
    db.sort_entries();

    // Show entries after sorting
    printf("\nAfter sorting (first 5 entries):\n");
    for (unsigned long i = 0; i < max_show; i++) {
        const BibEntry& entry = db.get_entry(i);
        printf("%lu. [%s] %s\n", i+1, 
               entry.get_year().c_str(), 
               entry.get_title().c_str());
    }

    printf("\nSorting completed. Entries are now ordered by year (desc) and title (asc).\n");
}

void demonstrate_merging() {
    printf("Creating two sample databases for merging demonstration...\n");

    // Create first database
    BibDatabase db1("Database 1");

    BibEntry entry1("sample2024");
    entry1.set_entry_type("article");
    entry1.set_title("Sample Article 2024");
    entry1.set_year("2024");
    Author author1("John Doe");
    entry1.add_author(author1);
    db1.add_entry(entry1);

    // Create second database
    BibDatabase db2("Database 2");

    BibEntry entry2("another2023");
    entry2.set_entry_type("inproceedings");
    entry2.set_title("Another Paper 2023");
    entry2.set_year("2023");
    Author author2("Jane Smith");
    entry2.add_author(author2);
    db2.add_entry(entry2);

    printf("Database 1 has %lu entries\n", db1.size());
    printf("Database 2 has %lu entries\n", db2.size());

    // Merge databases using + operator
    BibDatabase merged = db1 + db2;
    merged.set_name("Merged Database");

    printf("\nAfter merging with '+' operator:\n");
    printf("Merged database has %lu entries\n", merged.size());

    // Demonstrate += operator
    BibEntry entry3("third2025");
    entry3.set_entry_type("book");
    entry3.set_title("Third Entry 2025");
    entry3.set_year("2025");
    Author author3("Bob Johnson");
    entry3.add_author(author3);

    BibDatabase db3("Database 3");
    db3.add_entry(entry3);

    merged += db3;

    printf("After += operation with Database 3:\n");
    printf("Merged database now has %lu entries\n", merged.size());

    printf("\nMerging demonstration completed.\n");
}
