// Author.h - Author class following original C logic (corrected)
#ifndef AUTHOR_H
#define AUTHOR_H

#include "String.h"
#include "SystemInterface.h"

class Author {
private:
    String name;
    String affiliation;

public:
    // Constructors
    Author() : name(), affiliation() {}

    Author(const String& author_name, const String& author_affiliation = String())
        : name(author_name), affiliation(author_affiliation) {}

    Author(const char* author_name, const char* author_affiliation = "")
        : name(author_name ? author_name : ""), affiliation(author_affiliation ? author_affiliation : "") {}

    // Copy constructor
    Author(const Author& other) : name(other.name), affiliation(other.affiliation) {}

    // Destructor
    ~Author() {}

    // Assignment operator
    Author& operator=(const Author& other) {
        if (this != &other) {
            name = other.name;
            affiliation = other.affiliation;
        }
        return *this;
    }

    // Getters (return const references to avoid unnecessary copies)
    const String& getName() const { return name; }
    const String& getAffiliation() const { return affiliation; }

    // Setters
    void setName(const String& author_name) { name = author_name; }
    void setAffiliation(const String& author_affiliation) { affiliation = author_affiliation; }

    // Institute checking (following original C logic with improvements)
    bool isFromInstitute(const String& institute_name) const {
        if (institute_name.empty()) return false;

        // Compare lowercase forms
        String name_lower = name.to_lowercase();
        String institute_lower = institute_name.to_lowercase();

        int pos = name_lower.find(institute_lower);
        return pos >= 0;
    }

    // I/O functions
    void print() const {
        name.print();
        if (!affiliation.empty()) {
            SystemInterface::write(STDOUT_FILENO, " (", 2);
            affiliation.print();
            SystemInterface::write(STDOUT_FILENO, ")", 1);
        }
    }

    void println() const {
        print();
        SystemInterface::write(STDOUT_FILENO, "\n", 1);
    }

    // Validation
    bool isValid() const {
        return !name.empty();
    }

    // Comparison for sorting
    bool operator<(const Author& other) const {
        return name < other.name;
    }

    bool operator==(const Author& other) const {
        return name == other.name && affiliation == other.affiliation;
    }
};

#endif // AUTHOR_H
