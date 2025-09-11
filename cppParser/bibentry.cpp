// bibentry.cpp - Bibliography entry class implementation
#include "bibentry.h"
#include "author.h"

// Constructors
BibEntry::BibEntry() : author_count(0) {
    authors = nullptr;
    initialize();
}

BibEntry::BibEntry(const MyString& key) : entry_key(key), author_count(0) {
    authors = nullptr;
    initialize();
}

BibEntry::BibEntry(const BibEntry& other) : author_count(0) {
    authors = nullptr;
    initialize();
    *this = other;
}

// Destructor
BibEntry::~BibEntry() {
    clear_authors();
}

// Private helper methods
void BibEntry::initialize() {
    entry_type.clear();
    entry_key.clear();
    title.clear();
    year.clear();
    booktitle.clear();
    journal.clear();
    doi.clear();
    abstract.clear();
    pdf_url.clear();
    code_url.clear();
    ppt_url.clear();
    abbr.clear();
    pages.clear();
    volume.clear();
    number.clear();
    publisher.clear();
    address.clear();
    author_count = 0;

    // Allocate authors array
    if (!authors) {
        authors = (Author*)malloc(sizeof(Author) * MAX_AUTHORS);
        if (authors) {
            for (int i = 0; i < MAX_AUTHORS; i++) {
                new(authors + i) Author();
            }
        }
    }
}

// Assignment operator
BibEntry& BibEntry::operator=(const BibEntry& other) {
    if (this != &other) {
        entry_type = other.entry_type;
        entry_key = other.entry_key;
        title = other.title;
        year = other.year;
        booktitle = other.booktitle;
        journal = other.journal;
        doi = other.doi;
        abstract = other.abstract;
        pdf_url = other.pdf_url;
        code_url = other.code_url;
        ppt_url = other.ppt_url;
        abbr = other.abbr;
        pages = other.pages;
        volume = other.volume;
        number = other.number;
        publisher = other.publisher;
        address = other.address;

        // Clear existing authors
        clear_authors();

        // Copy authors
        author_count = other.author_count;
        if (author_count > 0 && authors) {
            for (int i = 0; i < author_count; i++) {
                authors[i] = other.authors[i];
            }
        }
    }
    return *this;
}

// Comparison operators for sorting by <year descending, title alphabetical>
bool BibEntry::operator<(const BibEntry& other) const {
    int this_year = get_year_as_int();
    int other_year = other.get_year_as_int();

    // First sort by year in descending order (newer years first)
    if (this_year != other_year) {
        return this_year > other_year; // Note: > for descending order
    }

    // If years are equal, sort by title alphabetically (ascending)
    return title < other.title;
}

bool BibEntry::operator>(const BibEntry& other) const {
    return other < *this;
}

bool BibEntry::operator==(const BibEntry& other) const {
    return entry_key == other.entry_key;
}

bool BibEntry::operator!=(const BibEntry& other) const {
    return !(*this == other);
}

// Accessors
const MyString& BibEntry::get_entry_type() const { return entry_type; }
const MyString& BibEntry::get_entry_key() const { return entry_key; }
const MyString& BibEntry::get_title() const { return title; }
const MyString& BibEntry::get_year() const { return year; }
const MyString& BibEntry::get_booktitle() const { return booktitle; }
const MyString& BibEntry::get_journal() const { return journal; }
const MyString& BibEntry::get_doi() const { return doi; }
const MyString& BibEntry::get_abstract() const { return abstract; }
const MyString& BibEntry::get_pdf_url() const { return pdf_url; }
const MyString& BibEntry::get_code_url() const { return code_url; }
const MyString& BibEntry::get_ppt_url() const { return ppt_url; }
int BibEntry::get_author_count() const { return author_count; }

const Author& BibEntry::get_author(int index) const {
    static Author empty_author; // Return empty author for invalid index
    if (index >= 0 && index < author_count && authors) {
        return authors[index];
    }
    return empty_author;
}

// Mutators
void BibEntry::set_entry_type(const MyString& type) { entry_type = type; }
void BibEntry::set_entry_key(const MyString& key) { entry_key = key; }
void BibEntry::set_title(const MyString& entry_title) { title = entry_title; }
void BibEntry::set_year(const MyString& entry_year) { 
    if (validate_year(entry_year)) {
        year = entry_year; 
    }
}
void BibEntry::set_booktitle(const MyString& entry_booktitle) { booktitle = entry_booktitle; }
void BibEntry::set_journal(const MyString& entry_journal) { journal = entry_journal; }
void BibEntry::set_doi(const MyString& entry_doi) { 
    if (validate_doi(entry_doi)) {
        doi = entry_doi; 
    }
}
void BibEntry::set_abstract(const MyString& entry_abstract) { abstract = entry_abstract; }
void BibEntry::set_pdf_url(const MyString& url) { 
    if (validate_url(url)) {
        pdf_url = url; 
    }
}
void BibEntry::set_code_url(const MyString& url) { 
    if (validate_url(url)) {
        code_url = url; 
    }
}
void BibEntry::set_ppt_url(const MyString& url) { 
    if (validate_url(url)) {
        ppt_url = url; 
    }
}

// Author management
void BibEntry::add_author(const Author& author) {
    if (author_count < MAX_AUTHORS && authors) {
        authors[author_count] = author;
        author_count++;
    }
}

void BibEntry::clear_authors() {
    if (authors) {
        // Call destructors
        for (int i = 0; i < MAX_AUTHORS; i++) {
            authors[i].~Author();
        }
        free(authors);
        authors = nullptr;
    }
    author_count = 0;
}

int BibEntry::count_institute_authors(const MyString& institute_name) const {
    int count = 0;
    if (authors) {
        for (int i = 0; i < author_count; i++) {
            if (authors[i].is_from_institute(institute_name)) {
                count++;
            }
        }
    }
    return count;
}

// Parsing methods
bool BibEntry::parse_entry_header(const MyString& header_line) {
    MyString line = header_line;
    line.trim();

    if (line.empty() || line[0] != '@') {
        return false;
    }

    // Find the opening brace
    unsigned long brace_pos = line.find("{");
    if (brace_pos == line.length()) {
        return false;
    }

    // Extract entry type
    entry_type = line.substr(1, brace_pos - 1);
    entry_type.trim();
    entry_type.to_lower();

    // Extract entry key
    unsigned long key_start = brace_pos + 1;
    unsigned long comma_pos = line.find(",", key_start);
    if (comma_pos == line.length()) {
        // No comma found, key extends to end or closing brace
        unsigned long close_brace = line.find("}", key_start);
        if (close_brace != line.length()) {
            entry_key = line.substr(key_start, close_brace - key_start);
        } else {
            entry_key = line.substr(key_start);
        }
    } else {
        entry_key = line.substr(key_start, comma_pos - key_start);
    }

    entry_key.trim();

    return !entry_type.empty() && !entry_key.empty();
}

bool BibEntry::parse_field_line(const MyString& field_line) {
    MyString field_name, field_value;
    if (!parse_field_value(field_line, field_name, field_value)) {
        return false;
    }

    set_field(field_name, field_value);
    return true;
}

bool BibEntry::parse_field_value(const MyString& line, MyString& field_name, MyString& field_value) {
    MyString trimmed_line = line;
    trimmed_line.trim();

    if (trimmed_line.empty()) {
        return false;
    }

    // Find the equals sign
    unsigned long equals_pos = trimmed_line.find("=");
    if (equals_pos == trimmed_line.length()) {
        return false;
    }

    // Extract field name
    field_name = trimmed_line.substr(0, equals_pos);
    field_name.trim();
    field_name.to_lower();

    // Extract field value
    field_value = trimmed_line.substr(equals_pos + 1);
    field_value.trim();

    // Remove trailing comma if present
    if (!field_value.empty() && field_value[field_value.length() - 1] == ',') {
        field_value = field_value.substr(0, field_value.length() - 1);
        field_value.trim();
    }

    // Remove braces if present
    if (!field_value.empty() && field_value[0] == '{') {
        field_value = field_value.substr(1);
        if (!field_value.empty() && field_value[field_value.length() - 1] == '}') {
            field_value = field_value.substr(0, field_value.length() - 1);
        }
    }

    field_value.trim();
    return !field_name.empty();
}

void BibEntry::set_field(const MyString& field_name, const MyString& field_value) {
    if (field_name == "title") {
        title = field_value;
    } else if (field_name == "author") {
        // Parse authors
        Author temp_authors[MAX_AUTHORS];
        int temp_count;
        if (Author::parse_author_field(field_value, temp_authors, MAX_AUTHORS, temp_count)) {
            clear_authors();
            initialize(); // Reallocate authors array
            for (int i = 0; i < temp_count; i++) {
                add_author(temp_authors[i]);
            }
        }
    } else if (field_name == "year") {
        set_year(field_value);
    } else if (field_name == "booktitle") {
        booktitle = field_value;
    } else if (field_name == "journal") {
        journal = field_value;
    } else if (field_name == "doi") {
        set_doi(field_value);
    } else if (field_name == "abstract") {
        abstract = field_value;
    } else if (field_name == "pdf") {
        set_pdf_url(field_value);
    } else if (field_name == "code") {
        set_code_url(field_value);
    } else if (field_name == "ppt") {
        set_ppt_url(field_value);
    } else if (field_name == "abbr") {
        abbr = field_value;
    } else if (field_name == "pages") {
        pages = field_value;
    } else if (field_name == "volume") {
        volume = field_value;
    } else if (field_name == "number") {
        number = field_value;
    } else if (field_name == "publisher") {
        publisher = field_value;
    } else if (field_name == "address") {
        address = field_value;
    }
}

// Validation methods
bool BibEntry::is_valid() const {
    return !entry_key.empty() && !title.empty() && !year.empty();
}

bool BibEntry::validate_year(const MyString& year_str) const {
    if (year_str.empty()) return false;

    // Check if all characters are digits
    for (unsigned long i = 0; i < year_str.length(); i++) {
        char c = year_str[i];
        if (c < '0' || c > '9') return false;
    }

    // Check reasonable range (1900-2100)
    int year_val = 0;
    for (unsigned long i = 0; i < year_str.length(); i++) {
        year_val = year_val * 10 + (year_str[i] - '0');
    }
    return year_val >= 1900 && year_val <= 2100;
}

bool BibEntry::validate_doi(const MyString& doi_str) const {
    if (doi_str.empty()) return true; // DOI is optional

    // Basic DOI format check: should start with "10."
    return doi_str.length() > 3 && 
           doi_str[0] == '1' && doi_str[1] == '0' && doi_str[2] == '.';
}

bool BibEntry::validate_url(const MyString& url_str) const {
    if (url_str.empty()) return true; // URLs are optional

    // Basic URL format check: should start with "http://" or "https://"
    return (url_str.length() > 7 && 
            (url_str.substr(0, 7) == "http://" || url_str.substr(0, 8) == "https://"));
}

// Utility methods
MyString BibEntry::to_string() const {
    MyString result = "@";
    result += entry_type;
    result += "{";
    result += entry_key;
    result += ",\n";

    if (!title.empty()) {
        result += "    title = {";
        result += title;
        result += "},\n";
    }

    if (author_count > 0) {
        result += "    author = {";
        result += get_formatted_authors();
        result += "},\n";
    }

    if (!year.empty()) {
        result += "    year = {";
        result += year;
        result += "},\n";
    }

    if (!booktitle.empty()) {
        result += "    booktitle = {";
        result += booktitle;
        result += "},\n";
    }

    if (!journal.empty()) {
        result += "    journal = {";
        result += journal;
        result += "},\n";
    }

    if (!doi.empty()) {
        result += "    doi = {";
        result += doi;
        result += "},\n";
    }

    if (!pdf_url.empty()) {
        result += "    pdf = {";
        result += pdf_url;
        result += "},\n";
    }

    if (!code_url.empty()) {
        result += "    code = {";
        result += code_url;
        result += "},\n";
    }

    if (!ppt_url.empty()) {
        result += "    ppt = {";
        result += ppt_url;
        result += "},\n";
    }

    if (!abstract.empty()) {
        result += "    abstract = {";
        // Truncate abstract for display
        if (abstract.length() > 100) {
            result += abstract.substr(0, 100);
            result += "...";
        } else {
            result += abstract;
        }
        result += "},\n";
    }

    result += "}\n";
    return result;
}

MyString BibEntry::get_formatted_authors() const {
    if (author_count == 0 || !authors) return MyString();

    MyString result = authors[0].get_name();

    for (int i = 1; i < author_count; i++) {
        result += " and ";
        result += authors[i].get_name();
    }

    return result;
}

bool BibEntry::empty() const {
    return entry_key.empty() && title.empty() && year.empty() && author_count == 0;
}

void BibEntry::clear() {
    clear_authors();
    initialize();
}

int BibEntry::get_year_as_int() const {
    if (year.empty()) return 0;

    int result = 0;
    for (unsigned long i = 0; i < year.length(); i++) {
        char c = year[i];
        if (c >= '0' && c <= '9') {
            result = result * 10 + (c - '0');
        } else {
            return 0; // Invalid year format
        }
    }

    return result;
}
