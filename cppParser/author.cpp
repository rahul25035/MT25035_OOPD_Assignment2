// author.cpp - Author class implementation
#include "author.h"

// Constructors
Author::Author() : name(), affiliation() {}

Author::Author(const MyString& author_name) : name(author_name), affiliation() {}

Author::Author(const MyString& author_name, const MyString& author_affiliation) 
    : name(author_name), affiliation(author_affiliation) {}

Author::Author(const Author& other) : name(other.name), affiliation(other.affiliation) {}

// Destructor
Author::~Author() {
    // MyString destructor handles cleanup
}

// Assignment operator
Author& Author::operator=(const Author& other) {
    if (this != &other) {
        name = other.name;
        affiliation = other.affiliation;
    }
    return *this;
}

// Comparison operators
bool Author::operator==(const Author& other) const {
    return name == other.name && affiliation == other.affiliation;
}

bool Author::operator!=(const Author& other) const {
    return !(*this == other);
}

bool Author::operator<(const Author& other) const {
    if (name != other.name) {
        return name < other.name;
    }
    return affiliation < other.affiliation;
}

// Accessors
const MyString& Author::get_name() const {
    return name;
}

const MyString& Author::get_affiliation() const {
    return affiliation;
}

// Mutators
void Author::set_name(const MyString& author_name) {
    name = author_name;
}

void Author::set_affiliation(const MyString& author_affiliation) {
    affiliation = author_affiliation;
}

// Utility methods
bool Author::is_from_institute(const MyString& institute_name) const {
    if (institute_name.empty()) return false;

    // Create lowercase versions for comparison
    MyString name_lower = name;
    name_lower.to_lower();

    MyString institute_lower = institute_name;
    institute_lower.to_lower();

    // Simple check - look for institute name in author name or affiliation
    // In real implementation, this could be more sophisticated
    bool found_in_name = (name_lower.find(institute_lower) != name_lower.length());

    bool found_in_affiliation = false;
    if (!affiliation.empty()) {
        MyString affil_lower = affiliation;
        affil_lower.to_lower();
        found_in_affiliation = (affil_lower.find(institute_lower) != affil_lower.length());
    }

    return found_in_name || found_in_affiliation;
}

MyString Author::to_string() const {
    if (affiliation.empty()) {
        return name;
    } else {
        return name + MyString(" (") + affiliation + MyString(")");
    }
}

bool Author::empty() const {
    return name.empty() && affiliation.empty();
}

void Author::clear() {
    name.clear();
    affiliation.clear();
}

// Static parsing method
bool Author::parse_author_field(const MyString& author_field, Author* authors, 
                               int max_authors, int& author_count) {
    if (author_field.empty() || !authors) {
        author_count = 0;
        return false;
    }

    author_count = 0;
    MyString field_copy = author_field;
    field_copy.trim();

    // Remove braces if present
    if (!field_copy.empty() && field_copy[0] == '{') {
        field_copy = field_copy.substr(1);
        if (!field_copy.empty() && field_copy[field_copy.length() - 1] == '}') {
            field_copy = field_copy.substr(0, field_copy.length() - 1);
        }
    }

    // Split by " and " to separate authors
    unsigned long pos = 0;
    while (pos < field_copy.length() && author_count < max_authors) {
        // Find next " and "
        unsigned long next_and = field_copy.find(" and ", pos);

        MyString author_name;
        if (next_and == field_copy.length()) {
            // Last author
            author_name = field_copy.substr(pos);
        } else {
            // Extract author name up to " and "
            author_name = field_copy.substr(pos, next_and - pos);
        }

        author_name.trim();
        if (!author_name.empty()) {
            authors[author_count] = Author(author_name);
            author_count++;
        }

        if (next_and == field_copy.length()) break;
        pos = next_and + 5; // Skip " and "
    }

    return author_count > 0;
}
