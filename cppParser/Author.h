// author.h - Author class definition
#ifndef AUTHOR_H
#define AUTHOR_H

#include "mystring.h"

class Author {
private:
    MyString name;
    MyString affiliation;

public:
    // Constructors
    Author();
    Author(const MyString& author_name);
    Author(const MyString& author_name, const MyString& author_affiliation);
    Author(const Author& other);

    // Destructor
    ~Author();

    // Assignment operator
    Author& operator=(const Author& other);

    // Comparison operators for sorting
    bool operator==(const Author& other) const;
    bool operator!=(const Author& other) const;
    bool operator<(const Author& other) const;

    // Accessors
    const MyString& get_name() const;
    const MyString& get_affiliation() const;

    // Mutators
    void set_name(const MyString& author_name);
    void set_affiliation(const MyString& author_affiliation);

    // Utility methods
    bool is_from_institute(const MyString& institute_name) const;
    MyString to_string() const;
    bool empty() const;
    void clear();

    // Static parsing method
    static bool parse_author_field(const MyString& author_field, Author* authors, 
                                  int max_authors, int& author_count);
};

#endif // AUTHOR_H
