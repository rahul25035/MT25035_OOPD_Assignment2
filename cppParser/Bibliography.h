// Bibliography.h - Bibliography management with + operator (fixed)
#ifndef BIBLIOGRAPHY_H
#define BIBLIOGRAPHY_H

#include "String.h"
#include "Publication.h"
#include "SystemInterface.h"
#include <new>       // placement new
#include <cstddef>
#include <cstdint>

class Bibliography {
private:
    Publication* publications;
    int publicationCount;
    int publicationCapacity;

    void ensureCapacity() {
        if (publicationCount < publicationCapacity) return;

        int newCapacity = (publicationCapacity == 0) ? 4 : publicationCapacity * 2;
        long bytes = (long)(newCapacity * sizeof(Publication));

        void* raw = SystemInterface::sbrk(bytes);
        if (raw == (void*)-1) {
            SystemInterface::write(STDERR_FILENO, "Memory allocation failed\n", 25);
            SystemInterface::exit(1);
        }

        Publication* newPubs = (Publication*)raw;

        // Construct Publication objects in-place for the whole new block
        for (int i = 0; i < newCapacity; i++) {
            new (newPubs + i) Publication();
        }

        // If there's an existing array, copy existing publications into the new block
        if (publications != nullptr && publicationCount > 0) {
            for (int i = 0; i < publicationCount; i++) {
                newPubs[i] = publications[i]; // uses Publication::operator=
            }
        }

        // Note: previous 'publications' memory cannot be reclaimed in this simple sbrk model.
        publications = newPubs;
        publicationCapacity = newCapacity;
    }

    // Sorting implementation
    void quickSort(int low, int high) {
        if (low < high) {
            int pi = partition(low, high);
            quickSort(low, pi - 1);
            quickSort(pi + 1, high);
        }
    }

    int partition(int low, int high) {
        int pivot = high;
        int i = low - 1;

        for (int j = low; j < high; j++) {
            if (publications[j] < publications[pivot]) {
                i++;
                swap(i, j);
            }
        }
        swap(i + 1, high);
        return i + 1;
    }

    void swap(int i, int j) {
        Publication temp = publications[i];
        publications[i] = publications[j];
        publications[j] = temp;
    }

    // BibTeX parsing helper functions
    // Extract field value after 'fieldName' in the line. Returns empty String if not found.
    String extractFieldValue(const String& line, const String& fieldName) {
        if (line.empty() || fieldName.empty()) return String();

        // Find the field name followed by optional whitespace and '='
        int pos = line.find(fieldName);
        if (pos == -1) return String();

        const char* s = line.c_str();
        int n = (int)line.size();

        // Move to the '=' after the field name
        int eq = -1;
        for (int i = pos + (int)fieldName.size(); i < n; i++) {
            if (s[i] == '=') {
                eq = i;
                break;
            }
            // skip whitespace between field name and '='
            if (s[i] != ' ' && s[i] != '\t') break;
        }
        if (eq == -1) return String();

        // Move to start of value (skip whitespace and opening { or " )
        int start = eq + 1;
        while (start < n && (s[start] == ' ' || s[start] == '\t')) start++;
        bool brace = false;
        if (start < n && (s[start] == '{' || s[start] == '\"')) {
            brace = true;
            start++;
        }

        // Find end of value: matching brace or comma/closing brace or closing quote
        int end = start;
        if (brace) {
            // look for closing brace - be simple: find next '}' in this line
            while (end < n && s[end] != '}') end++;
        } else {
            while (end < n && s[end] != ',' && s[end] != '}' && s[end] != '\n' && s[end] != '\r') end++;
        }

        if (end <= start) return String();

        // Create substring and trim
        String value = line.substr((size_t)start, (size_t)(end - start)).trim();
        return value;
    }

    // Extract authors splitting on " and " (original C logic)
    void extractAuthors(const String& authorField, Publication& pub) {
        String remaining = authorField;

        // Remove outer braces if present
        const char* rs = remaining.c_str();
        if (rs[0] == '{') {
            remaining = remaining.substr(1);
            if (remaining.size() > 0 && remaining[remaining.size() - 1] == '}') {
                remaining = remaining.substr(0, remaining.size() - 1);
            }
        }

        // Split by " and "
        while (!remaining.empty()) {
            int andPos = remaining.find(" and ");
            String authorName;
            if (andPos != -1) {
                authorName = remaining.substr(0, (size_t)andPos).trim();
                remaining = remaining.substr((size_t)andPos + 5).trim();
            } else {
                authorName = remaining.trim();
                remaining = String();
            }

            if (!authorName.empty()) {
                pub.addAuthor(Author(authorName));
            }
        }
    }

public:
    // Constructors
    Bibliography() : publications(nullptr), publicationCount(0), publicationCapacity(0) {}

    // Copy constructor
    Bibliography(const Bibliography& other) {
        publications = nullptr;
        publicationCount = 0;
        publicationCapacity = 0;

        for (int i = 0; i < other.publicationCount; i++) {
            addPublication(other.publications[i]);
        }
    }

    // Destructor
    ~Bibliography() {
        // Cannot free sbrk'd memory here; if you want to run destructors:
        // for (int i = 0; i < publicationCount; ++i) publications[i].~Publication();
    }

    // Assignment operator
    Bibliography& operator=(const Bibliography& other) {
        if (this != &other) {
            publicationCount = 0;
            for (int i = 0; i < other.publicationCount; i++) {
                addPublication(other.publications[i]);
            }
        }
        return *this;
    }

    // + operator for combining bibliographies
    Bibliography operator+(const Bibliography& other) const {
        Bibliography result(*this);
        result += other;
        return result;
    }

    Bibliography& operator+=(const Bibliography& other) {
        for (int i = 0; i < other.publicationCount; i++) {
            addPublication(other.publications[i]);
        }
        sort(); // Re-sort after adding
        return *this;
    }

    // Publication management
    void addPublication(const Publication& pub) {
        ensureCapacity();
        publications[publicationCount] = pub; // uses Publication::operator=
        publicationCount++;
    }

    int getPublicationCount() const { return publicationCount; }

    Publication getPublication(int index) const {
        if (index < 0 || index >= publicationCount) {
            SystemInterface::write(STDERR_FILENO, "Publication index out of bounds\n", 32);
            SystemInterface::exit(1);
        }
        return publications[index];
    }

    // Sorting
    void sort() {
        if (publicationCount > 1) {
            quickSort(0, publicationCount - 1);
        }
    }

    // File I/O - parse BibTeX with fields similar to provided example
    bool loadFromBibFile(const String& filename) {
        int fd = SystemInterface::open(filename.c_str(), O_RDONLY);
        if (fd < 0) {
            SystemInterface::write(STDERR_FILENO, "Error: Cannot open file ", 24);
            filename.print();
            SystemInterface::write(STDERR_FILENO, "\n", 1);
            return false;
        }

        SystemInterface::write(STDOUT_FILENO, "Parsing BibTeX file: ", 21);
        filename.println();

        char buffer[1024];
        int bufPos = 0;
        String currentLine;
        Publication currentPub;
        bool inEntry = false;
        int totalEntries = 0;

        ssize_t bytesRead;
        char ch;
        while ((bytesRead = SystemInterface::read(fd, &ch, 1)) > 0) {
            if (ch == '\r') continue; // ignore CR
            if (ch == '\n' || bufPos >= 1023) {
                buffer[bufPos] = '\0';
                currentLine = String(buffer);
                String trimmedLine = currentLine.trim();

                if (!trimmedLine.empty()) {
                    const char* lineStr = trimmedLine.c_str();

                    // Start of entry (e.g., @inproceedings{... ,)
                    if (lineStr[0] == '@') {
                        if (inEntry && currentPub.isValid()) {
                            addPublication(currentPub);
                            currentPub = Publication();
                        }
                        inEntry = true;
                        totalEntries++;
                    }
                    // End of entry (a line with single '}' usually ends the entry)
                    else if (inEntry && lineStr[0] == '}' && trimmedLine.size() == 1) {
                        if (currentPub.isValid()) {
                            addPublication(currentPub);
                        }
                        currentPub = Publication();
                        inEntry = false;
                    }
                    // Parse fields within an entry
                    else if (inEntry) {
                        // Title
                        String titleValue = extractFieldValue(trimmedLine, String("title"));
                        if (!titleValue.empty()) {
                            currentPub.setTitle(titleValue);
                        }

                        // Year
                        String yearValue = extractFieldValue(trimmedLine, String("year"));
                        if (!yearValue.empty()) {
                            currentPub.setYear(yearValue);
                        }

                        // Journal
                        String journalValue = extractFieldValue(trimmedLine, String("journal"));
                        if (!journalValue.empty()) {
                            currentPub.setJournal(journalValue);
                        }

                        // Booktitle -> map to journal if journal empty
                        String booktitleValue = extractFieldValue(trimmedLine, String("booktitle"));
                        if (!booktitleValue.empty() && currentPub.getJournal().empty()) {
                            currentPub.setJournal(booktitleValue);
                        }

                        // Author(s)
                        String authorValue = extractFieldValue(trimmedLine, String("author"));
                        if (!authorValue.empty()) {
                            extractAuthors(authorValue, currentPub);
                        }

                        // PDF or URL
                        String pdfValue = extractFieldValue(trimmedLine, String("pdf"));
                        if (pdfValue.empty()) {
                            pdfValue = extractFieldValue(trimmedLine, String("url"));
                        }
                        if (!pdfValue.empty()) {
                            currentPub.setPdfUrl(pdfValue);
                        }

                        // Code link
                        String codeValue = extractFieldValue(trimmedLine, String("code"));
                        if (!codeValue.empty()) {
                            currentPub.setSourceCodeUrl(codeValue);
                        }

                        // Slides/presentation - accept both "slides" and "ppt"
                        String slidesValue = extractFieldValue(trimmedLine, String("slides"));
                        if (slidesValue.empty()) {
                            slidesValue = extractFieldValue(trimmedLine, String("ppt"));
                        }
                        if (!slidesValue.empty()) {
                            currentPub.setPresentationUrl(slidesValue);
                        }

                        // Note: abstract and other fields are currently ignored (you can extend to store them in Publication)
                    }
                }

                bufPos = 0;
            } else {
                buffer[bufPos++] = ch;
            }
        }

        // If EOF reached while still in an entry, commit the last publication
        if (inEntry && currentPub.isValid()) {
            addPublication(currentPub);
        }

        SystemInterface::close(fd);
        sort(); // Sort after loading

        // Print summary
        SystemInterface::write(STDOUT_FILENO, "Loaded ", 7);
        printNumber(publicationCount);
        SystemInterface::write(STDOUT_FILENO, " publications\n", 14);

        return true;
    }

    // Institute counting: counts and prints authors from a given institute
    int countAuthorsFromInstitute(const String& instituteName) const {
        int totalCount = 0;

        SystemInterface::write(STDOUT_FILENO, "Looking for authors from: ", 26);
        instituteName.println();
        SystemInterface::write(STDOUT_FILENO, "\n", 1);

        for (int i = 0; i < publicationCount; i++) {
            int count = publications[i].countAuthorsFromInstitute(instituteName);
            if (count > 0) {
                // Print found authors
                for (int j = 0; j < publications[i].getAuthorCount(); j++) {
                    Author author = publications[i].getAuthor(j);
                    if (author.isFromInstitute(instituteName)) {
                        SystemInterface::write(STDOUT_FILENO, "Found institute author: ", 24);
                        author.println();
                    }
                }

                SystemInterface::write(STDOUT_FILENO, "Entry ", 6);
                printNumber(i + 1);
                SystemInterface::write(STDOUT_FILENO, " has ", 5);
                printNumber(count);
                SystemInterface::write(STDOUT_FILENO, " author(s) from ", 16);
                instituteName.println();

                totalCount += count;
            }
        }

        return totalCount;
    }

    // Helper to print numbers (no printf)
    void printNumber(int n) const {
        if (n == 0) {
            SystemInterface::write(STDOUT_FILENO, "0", 1);
            return;
        }

        char buffer[32];
        int i = 0;
        bool negative = false;

        if (n < 0) {
            negative = true;
            n = -n;
        }

        while (n > 0) {
            buffer[i++] = '0' + (n % 10);
            n /= 10;
        }

        if (negative) {
            buffer[i++] = '-';
        }

        // Reverse and print
        for (int j = i - 1; j >= 0; j--) {
            SystemInterface::write(STDOUT_FILENO, &buffer[j], 1);
        }
    }

    // Display functions
    void print() const {
        for (int i = 0; i < publicationCount; i++) {
            printNumber(i + 1);
            SystemInterface::write(STDOUT_FILENO, ". ", 2);
            publications[i].println();
        }
    }

    void printDetailed() const {
        SystemInterface::write(STDOUT_FILENO, "=== Bibliography (", 18);
        printNumber(publicationCount);
        SystemInterface::write(STDOUT_FILENO, " publications) ===\n\n", 20);

        for (int i = 0; i < publicationCount; i++) {
            printNumber(i + 1);
            SystemInterface::write(STDOUT_FILENO, ". ", 2);
            publications[i].printDetailed();
        }
    }

    void printSummary(const String& instituteName) const {
        SystemInterface::write(STDOUT_FILENO, "\n=== Summary ===\n", 17);
        SystemInterface::write(STDOUT_FILENO, "Total BibTeX entries processed: ", 32);
        printNumber(publicationCount);
        SystemInterface::write(STDOUT_FILENO, "\n", 1);

        int totalAuthors = countAuthorsFromInstitute(instituteName);
        SystemInterface::write(STDOUT_FILENO, "Total authors from ", 19);
        instituteName.print();
        SystemInterface::write(STDOUT_FILENO, ": ", 2);
        printNumber(totalAuthors);
        SystemInterface::write(STDOUT_FILENO, "\n", 1);
    }

    bool isEmpty() const { return publicationCount == 0; }
};

#endif // BIBLIOGRAPHY_H
