// bibdatabase.cpp - Bibliography database class implementation
#include "bibdatabase.h"

// System calls for file I/O
extern "C" {
    int open(const char* path, int flags, ...);
    int close(int fd);
    long read(int fd, void* buf, unsigned long count);
    long write(int fd, const void* buf, unsigned long count);
    int printf(const char* format, ...);
}

// File flags
#ifndef O_RDONLY
#define O_RDONLY 0
#endif
#ifndef O_WRONLY
#define O_WRONLY 1
#endif
#ifndef O_CREAT
#define O_CREAT 64
#endif
#ifndef O_TRUNC
#define O_TRUNC 512
#endif

// Constructors
BibDatabase::BibDatabase() : entries(), database_name("Unnamed Database") {}

BibDatabase::BibDatabase(const MyString& name) : entries(), database_name(name) {}

BibDatabase::BibDatabase(const BibDatabase& other) : entries(other.entries), database_name(other.database_name) {}

// Destructor
BibDatabase::~BibDatabase() {
    // MyVector destructor handles cleanup
}

// Assignment operator
BibDatabase& BibDatabase::operator=(const BibDatabase& other) {
    if (this != &other) {
        entries = other.entries;
        database_name = other.database_name;
    }
    return *this;
}

// Addition operators for merging databases
BibDatabase BibDatabase::operator+(const BibDatabase& other) const {
    BibDatabase result = *this;
    result += other;
    return result;
}

BibDatabase& BibDatabase::operator+=(const BibDatabase& other) {
    for (unsigned long i = 0; i < other.entries.get_size(); i++) {
        const BibEntry& entry = other.entries[i];
        // Check if entry with same key already exists
        if (find_entry(entry.get_entry_key()) == nullptr) {
            add_entry(entry);
        }
    }
    return *this;
}

// File I/O helper methods
bool BibDatabase::read_line(int fd, char* buffer, int max_len) {
    int pos = 0;
    char c;

    while (pos < max_len - 1) {
        long bytes_read = read(fd, &c, 1);
        if (bytes_read <= 0) {
            // End of file or error
            break;
        }

        if (c == '\n') {
            // End of line
            break;
        }

        buffer[pos++] = c;
    }

    buffer[pos] = '\0';
    return pos > 0;
}

bool BibDatabase::is_whitespace_line(const MyString& line) {
    MyString trimmed = line;
    trimmed.trim();
    return trimmed.empty();
}

// File operations
bool BibDatabase::load_from_file(const MyString& filename) {
    if (filename.empty()) {
        printf("Error: Empty filename\n");
        return false;
    }

    int fd = open(filename.c_str(), O_RDONLY);
    if (fd < 0) {
        printf("Error: Cannot open file %s\n", filename.c_str());
        return false;
    }

    clear(); // Clear existing entries

    char line_buffer[MAX_LINE_LENGTH];
    MyString current_line;
    int total_entries = 0;

    printf("Parsing BibTeX file: %s\n", filename.c_str());

    while (read_line(fd, line_buffer, MAX_LINE_LENGTH)) {
        current_line = MyString(line_buffer);

        // Skip empty lines and comments
        if (is_whitespace_line(current_line)) {
            continue;
        }

        // Check if this is the start of a new entry
        MyString trimmed_line = current_line;
        trimmed_line.trim();

        if (!trimmed_line.empty() && trimmed_line[0] == '@') {
            // Parse this entry
            if (parse_bib_entry(fd, current_line)) {
                total_entries++;
            }
        }
    }

    close(fd);

    printf("\n=== Summary ===\n");
    printf("Total BibTeX entries processed: %d\n", total_entries);

    return total_entries > 0;
}

bool BibDatabase::save_to_file(const MyString& filename) const {
    if (filename.empty()) return false;

    int fd = open(filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        printf("Error: Cannot create file %s\n", filename.c_str());
        return false;
    }

    for (unsigned long i = 0; i < entries.get_size(); i++) {
        MyString entry_str = entries[i].to_string();
        write(fd, entry_str.c_str(), entry_str.length());
        write(fd, "\n", 1);
    }

    close(fd);
    return true;
}

bool BibDatabase::parse_bib_entry(int fd, MyString& current_line) {
    BibEntry entry;

    // Parse the entry header (@inproceedings{key, etc.)
    if (!entry.parse_entry_header(current_line)) {
        printf("Warning: Failed to parse entry header: %s\n", current_line.c_str());
        return false;
    }

    // Read and parse fields until we find the closing brace
    char line_buffer[MAX_LINE_LENGTH];
    int brace_level = 1; // We've seen one opening brace

    while (brace_level > 0 && read_line(fd, line_buffer, MAX_LINE_LENGTH)) {
        MyString line(line_buffer);
        MyString trimmed_line = line;
        trimmed_line.trim();

        // Skip empty lines
        if (trimmed_line.empty()) {
            continue;
        }

        // Count braces to handle nested structures
        for (unsigned long i = 0; i < trimmed_line.length(); i++) {
            if (trimmed_line[i] == '{') {
                brace_level++;
            } else if (trimmed_line[i] == '}') {
                brace_level--;
            }
        }

        // If this is just a closing brace, we're done
        if (trimmed_line == "}") {
            break;
        }

        // Try to parse this as a field line
        if (brace_level == 1) { // Only parse fields at the top level
            entry.parse_field_line(line);
        }
    }

    // Add the entry if it's valid
    if (entry.is_valid()) {
        add_entry(entry);
        printf("Added entry: %s\n", entry.get_entry_key().c_str());
        return true;
    } else {
        printf("Warning: Invalid entry skipped\n");
        return false;
    }
}

// Entry management
void BibDatabase::add_entry(const BibEntry& entry) {
    entries.push_back(entry);
}

bool BibDatabase::remove_entry(const MyString& entry_key) {
    // Simple removal - create new vector without the entry
    MyVector<BibEntry> new_entries;
    bool found = false;

    for (unsigned long i = 0; i < entries.get_size(); i++) {
        if (entries[i].get_entry_key() != entry_key) {
            new_entries.push_back(entries[i]);
        } else {
            found = true;
        }
    }

    if (found) {
        entries = new_entries;
    }

    return found;
}

BibEntry* BibDatabase::find_entry(const MyString& entry_key) {
    for (unsigned long i = 0; i < entries.get_size(); i++) {
        if (entries[i].get_entry_key() == entry_key) {
            return &entries[i];
        }
    }
    return nullptr;
}

const BibEntry* BibDatabase::find_entry(const MyString& entry_key) const {
    for (unsigned long i = 0; i < entries.get_size(); i++) {
        if (entries[i].get_entry_key() == entry_key) {
            return &entries[i];
        }
    }
    return nullptr;
}

// Database operations
void BibDatabase::sort_entries() {
    entries.sort();
}

void BibDatabase::clear() {
    entries.clear();
}

bool BibDatabase::empty() const {
    return entries.empty();
}

unsigned long BibDatabase::size() const {
    return entries.get_size();
}

// Search and filter operations
int BibDatabase::count_institute_authors(const MyString& institute_name) const {
    int total_count = 0;

    printf("Looking for authors from: %s\n\n", institute_name.c_str());

    for (unsigned long i = 0; i < entries.get_size(); i++) {
        int entry_count = entries[i].count_institute_authors(institute_name);
        if (entry_count > 0) {
            printf("Entry '%s' has %d author(s) from %s\n", 
                   entries[i].get_entry_key().c_str(), 
                   entry_count, 
                   institute_name.c_str());

            // Print the authors from this institute
            for (int j = 0; j < entries[i].get_author_count(); j++) {
                const Author& author = entries[i].get_author(j);
                if (author.is_from_institute(institute_name)) {
                    printf("  Found institute author: %s\n", author.get_name().c_str());
                }
            }

            total_count += entry_count;
        }
    }

    return total_count;
}

// Accessors
const MyString& BibDatabase::get_name() const {
    return database_name;
}

void BibDatabase::set_name(const MyString& name) {
    database_name = name;
}

BibEntry& BibDatabase::get_entry(unsigned long index) {
    return entries[index];
}

const BibEntry& BibDatabase::get_entry(unsigned long index) const {
    return entries[index];
}

// Display methods
MyString BibDatabase::to_string() const {
    MyString result = "BibTeX Database: ";
    result += database_name;
    result += "\n";
    result += "Number of entries: ";

    // Convert size to string
    char size_str[20];
    unsigned long s = entries.get_size();
    int pos = 0;
    if (s == 0) {
        size_str[pos++] = '0';
    } else {
        char temp[20];
        int temp_pos = 0;
        while (s > 0) {
            temp[temp_pos++] = '0' + (s % 10);
            s /= 10;
        }
        for (int i = temp_pos - 1; i >= 0; i--) {
            size_str[pos++] = temp[i];
        }
    }
    size_str[pos] = '\0';

    result += MyString(size_str);
    result += "\n\n";

    return result;
}

void BibDatabase::print_summary() const {
    printf("%s", to_string().c_str());
}

void BibDatabase::print_entries() const {
    printf("=== Bibliography Entries ===\n");
    for (unsigned long i = 0; i < entries.get_size(); i++) {
        printf("Entry %lu:\n", i + 1);
        printf("Key: %s\n", entries[i].get_entry_key().c_str());
        printf("Title: %s\n", entries[i].get_title().c_str());
        printf("Year: %s\n", entries[i].get_year().c_str());
        printf("Authors: %s\n", entries[i].get_formatted_authors().c_str());
        printf("\n");
    }
}

void BibDatabase::print_institute_authors(const MyString& institute_name) const {
    int total = count_institute_authors(institute_name);
    printf("\nTotal authors from %s: %d\n", institute_name.c_str(), total);
}

// Validation
bool BibDatabase::validate() const {
    for (unsigned long i = 0; i < entries.get_size(); i++) {
        if (!entries[i].is_valid()) {
            return false;
        }
    }
    return true;
}
