// Publication.h - Publication class with URL fields
#ifndef PUBLICATION_H
#define PUBLICATION_H

#include "String.h"
#include "Author.h"
#include "SystemInterface.h"

// Manual placement new implementation
inline void* operator new(size_t size, void* ptr) {
    (void)size; // suppress unused parameter warning
    return ptr;
}

class Publication {
private:
    String title;
    String year;
    String journal;
    String volume;
    String pages;
    String pdfUrl;
    String sourceCodeUrl;
    String presentationUrl;
    Author* authors;
    int authorCount;
    int authorCapacity;

    void ensureAuthorCapacity() {
        if (authorCount < authorCapacity) return;
        
        int newCapacity = (authorCapacity == 0) ? 4 : authorCapacity * 2;
        size_t totalBytes = (size_t)newCapacity * sizeof(Author);
        long bytes = (long)totalBytes;

        void* raw = SystemInterface::sbrk(bytes);
        if (raw == (void*)-1) {
            SystemInterface::write(STDERR_FILENO, "Memory allocation failed\n", 25);
            SystemInterface::exit(1);
        }

        Author* newAuthors = (Author*)raw;

        // Construct Author objects in-place
        for (int i = 0; i < newCapacity; i++) {
            new (newAuthors + i) Author();
        }

        // Copy existing authors
        if (authors != (Author*)0 && authorCount > 0) {
            for (int i = 0; i < authorCount; i++) {
                newAuthors[i] = authors[i];
            }
        }

        authors = newAuthors;
        authorCapacity = newCapacity;
    }

public:
    // Constructors
    Publication()
        : title(), year(), journal(), volume(), pages(),
          pdfUrl(), sourceCodeUrl(), presentationUrl(),
          authors(nullptr), authorCount(0), authorCapacity(0) {}

    Publication(const String& pub_title, const String& pub_year = String())
        : title(pub_title), year(pub_year), journal(), volume(), pages(),
          pdfUrl(), sourceCodeUrl(), presentationUrl(),
          authors(nullptr), authorCount(0), authorCapacity(0) {}

    // Copy constructor
    Publication(const Publication& other)
        : title(other.title), year(other.year), journal(other.journal),
          volume(other.volume), pages(other.pages),
          pdfUrl(other.pdfUrl), sourceCodeUrl(other.sourceCodeUrl),
          presentationUrl(other.presentationUrl),
          authors(nullptr), authorCount(0), authorCapacity(0) {
        // Copy authors one by one using addAuthor to ensure capacity
        for (int i = 0; i < other.authorCount; i++) {
            addAuthor(other.authors[i]);
        }
    }

    // Destructor (can't free sbrk allocations in this model)
    ~Publication() {
    }

    // Assignment operator
    Publication& operator=(const Publication& other) {
        if (this != &other) {
            title = other.title;
            year = other.year;
            journal = other.journal;
            volume = other.volume;
            pages = other.pages;
            pdfUrl = other.pdfUrl;
            sourceCodeUrl = other.sourceCodeUrl;
            presentationUrl = other.presentationUrl;

            // Reset authors and copy
            authorCount = 0;
            // Note: we intentionally do not free old memory (sbrk model)
            for (int i = 0; i < other.authorCount; i++) {
                addAuthor(other.authors[i]);
            }
        }
        return *this;
    }

    // Setters
    void setTitle(const String& t) { title = t; }
    void setYear(const String& y) { year = y; }
    void setJournal(const String& j) { journal = j; }
    void setVolume(const String& v) { volume = v; }
    void setPages(const String& p) { pages = p; }
    void setPdfUrl(const String& url) { pdfUrl = url; }
    void setSourceCodeUrl(const String& url) { sourceCodeUrl = url; }
    void setPresentationUrl(const String& url) { presentationUrl = url; }

    // Getters
    String getTitle() const { return title; }
    String getYear() const { return year; }
    String getJournal() const { return journal; }
    String getVolume() const { return volume; }
    String getPages() const { return pages; }
    String getPdfUrl() const { return pdfUrl; }
    String getSourceCodeUrl() const { return sourceCodeUrl; }
    String getPresentationUrl() const { return presentationUrl; }

    // Author management
    void addAuthor(const Author& author) {
        ensureAuthorCapacity();
        authors[authorCount] = author;
        authorCount++;
    }

    int getAuthorCount() const { return authorCount; }

    Author getAuthor(int index) const {
        if (index < 0 || index >= authorCount) {
            SystemInterface::write(STDERR_FILENO, "Author index out of bounds\n", 27);
            SystemInterface::exit(1);
        }
        return authors[index];
    }

    // Institute counting (following original C logic)
    int countAuthorsFromInstitute(const String& institute_name) const {
        int count = 0;
        for (int i = 0; i < authorCount; i++) {
            if (authors[i].isFromInstitute(institute_name)) {
                count++;
            }
        }
        return count;
    }

    // Comparison operator for sorting (year desc, title asc as required)
    bool operator<(const Publication& other) const {
        // Parse years to integers (non-digits treated as 0)
        int thisYear = 0;
        int otherYear = 0;

        const char* thisYearStr = year.c_str();
        const char* otherYearStr = other.year.c_str();

        for (int i = 0; thisYearStr[i] >= '0' && thisYearStr[i] <= '9'; i++) {
            thisYear = thisYear * 10 + (thisYearStr[i] - '0');
        }
        for (int i = 0; otherYearStr[i] >= '0' && otherYearStr[i] <= '9'; i++) {
            otherYear = otherYear * 10 + (otherYearStr[i] - '0');
        }

        // For descending order by year (newer first)
        if (thisYear != otherYear) {
            return thisYear > otherYear;
        }

        // Title ascending (lexicographic)
        return title < other.title;
    }

    bool operator==(const Publication& other) const {
        return title == other.title && year == other.year;
    }

    // I/O functions
    void print() const {
        SystemInterface::write(STDOUT_FILENO, "[", 1);
        year.print();
        SystemInterface::write(STDOUT_FILENO, "] ", 2);
        title.print();
    }

    void println() const {
        print();
        SystemInterface::write(STDOUT_FILENO, "\n", 1);
    }

    void printDetailed() const {
        SystemInterface::write(STDOUT_FILENO, "Title: ", 7);
        title.println();
        SystemInterface::write(STDOUT_FILENO, "Year: ", 6);
        year.println();

        if (!journal.empty()) {
            SystemInterface::write(STDOUT_FILENO, "Journal: ", 9);
            journal.println();
        }

        if (authorCount > 0) {
            SystemInterface::write(STDOUT_FILENO, "Authors: ", 9);
            for (int i = 0; i < authorCount; i++) {
                if (i > 0) SystemInterface::write(STDOUT_FILENO, ", ", 2);
                authors[i].print();
            }
            SystemInterface::write(STDOUT_FILENO, "\n", 1);
        }

        if (!pdfUrl.empty()) {
            SystemInterface::write(STDOUT_FILENO, "PDF URL: ", 9);
            pdfUrl.println();
        }

        if (!sourceCodeUrl.empty()) {
            SystemInterface::write(STDOUT_FILENO, "Source Code URL: ", 17);
            sourceCodeUrl.println();
        }

        if (!presentationUrl.empty()) {
            SystemInterface::write(STDOUT_FILENO, "Presentation URL: ", 18);
            presentationUrl.println();
        }

        SystemInterface::write(STDOUT_FILENO, "\n", 1);
    }

    // Validation
    bool isValid() const {
        return !title.empty() && !year.empty();
    }
};

#endif // PUBLICATION_H
