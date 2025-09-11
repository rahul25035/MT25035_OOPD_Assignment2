// bibentry.h - Bibliography entry class definition
#ifndef BIBENTRY_H
#define BIBENTRY_H

#include "mystring.h"
#include "author.h"

class BibEntry {
private:
    MyString entry_type;    // @inproceedings, @article, etc.
    MyString entry_key;     // Citation key
    MyString title;
    MyString year;
    MyString booktitle;
    MyString journal;
    MyString doi;
    MyString abstract;

    // Additional URL fields as required by assignment
    MyString pdf_url;
    MyString code_url;
    MyString ppt_url;

    // Authors array
    static const int MAX_AUTHORS = 100;
    Author authors[MAX_AUTHORS];
    int author_count;

    // Additional fields
    MyString abbr;
    MyString pages;
    MyString volume;
    MyString number;
    MyString publisher;
    MyString address;

    // Private helper methods
    void initialize();
    bool parse_field_value(const MyString& line, MyString& field_name, MyString& field_value);
    void set_field(const MyString& field_name, const MyString& field_value);

public:
    // Constructors
    BibEntry();
    BibEntry(const MyString& key);
    BibEntry(const BibEntry& other);

    // Destructor
    ~BibEntry();

    // Assignment operator
    BibEntry& operator=(const BibEntry& other);

    // Comparison operators for sorting by <year, alphabetical>
    bool operator<(const BibEntry& other) const;
    bool operator>(const BibEntry& other) const;
    bool operator==(const BibEntry& other) const;
    bool operator!=(const BibEntry& other) const;

    // Accessors
    const MyString& get_entry_type() const;
    const MyString& get_entry_key() const;
    const MyString& get_title() const;
    const MyString& get_year() const;
    const MyString& get_booktitle() const;
    const MyString& get_journal() const;
    const MyString& get_doi() const;
    const MyString& get_abstract() const;
    const MyString& get_pdf_url() const;
    const MyString& get_code_url() const;
    const MyString& get_ppt_url() const;
    int get_author_count() const;
    const Author& get_author(int index) const;

    // Mutators
    void set_entry_type(const MyString& type);
    void set_entry_key(const MyString& key);
    void set_title(const MyString& entry_title);
    void set_year(const MyString& entry_year);
    void set_booktitle(const MyString& entry_booktitle);
    void set_journal(const MyString& entry_journal);
    void set_doi(const MyString& entry_doi);
    void set_abstract(const MyString& entry_abstract);
    void set_pdf_url(const MyString& url);
    void set_code_url(const MyString& url);
    void set_ppt_url(const MyString& url);

    // Author management
    void add_author(const Author& author);
    void clear_authors();
    int count_institute_authors(const MyString& institute_name) const;

    // Parsing methods
    bool parse_entry_header(const MyString& header_line);
    bool parse_field_line(const MyString& field_line);
    bool is_valid() const;

    // Utility methods
    MyString to_string() const;
    MyString get_formatted_authors() const;
    bool empty() const;
    void clear();

    // Year conversion utility
    int get_year_as_int() const;

    // Validation
    bool validate_year(const MyString& year_str) const;
    bool validate_doi(const MyString& doi_str) const;
    bool validate_url(const MyString& url_str) const;
};

#endif // BIBENTRY_H
