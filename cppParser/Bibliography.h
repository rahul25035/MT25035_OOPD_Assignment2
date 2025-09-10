// Bibliography.h - Bibliography management with + operator
#ifndef BIBLIOGRAPHY_H
#define BIBLIOGRAPHY_H

#include "String.h"
#include "Publication.h"

class Bibliography {
private:
    Publication* publications;
    int publicationCount;
    int publicationCapacity;

    void ensureCapacity() {
        if (publicationCount >= publicationCapacity) {
            int newCapacity = (publicationCapacity == 0) ? 4 : publicationCapacity * 2;

            Publication* newPubs = (Publication*)SystemInterface::sbrk(newCapacity * sizeof(Publication));
            if (!newPubs) {
                SystemInterface::write(STDERR_FILENO, "Memory allocation failed\n", 25);
                SystemInterface::exit(1);
            }

            // Use placement new
            for (int i = 0; i < newCapacity; i++) {
                new (newPubs + i) Publication();
            }

            // Copy existing publications
            for (int i = 0; i < publicationCount; i++) {
                newPubs[i] = publications[i];
            }

            publications = newPubs;
            publicationCapacity = newCapacity;
        }
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

    // BibTeX parsing helper functions (following original C logic)
    String extractFieldValue(const String& line, const String& fieldName) {
        int fieldPos = line.find(fieldName);
        if (fieldPos == -1) return String();

        // Find equals sign
        const char* lineStr = line.c_str();
        int equalPos = -1;
        for (size_t i = fieldPos; i < line.size(); i++) {
            if (lineStr[i] == '=') {
                equalPos = i;
                break;
            }
        }

        if (equalPos == -1) return String();

        // Find value start (skip whitespace and braces)
        int valueStart = equalPos + 1;
        while (valueStart < (int)line.size() && 
               (lineStr[valueStart] == ' ' || lineStr[valueStart] == '\t' || 
                lineStr[valueStart] == '{' || lineStr[valueStart] == '"')) {
            valueStart++;
        }

        // Find value end
        int valueEnd = valueStart;
        while (valueEnd < (int)line.size() && 
               lineStr[valueEnd] != ',' && lineStr[valueEnd] != '}' && 
               lineStr[valueEnd] != '"' && lineStr[valueEnd] != '\n') {
            valueEnd++;
        }

        if (valueEnd > valueStart) {
            return line.substr(valueStart, valueEnd - valueStart).trim();
        }

        return String();
    }

    // Extract authors following original C logic exactly
    void extractAuthors(const String& authorField, Publication& pub) {
        String remaining = authorField;

        // Remove outer braces if present (original C logic)
        const char* remainingStr = remaining.c_str();
        if (remainingStr[0] == '{') {
            remaining = remaining.substr(1);
            if (remaining.size() > 0 && remaining[remaining.size()-1] == '}') {
                remaining = remaining.substr(0, remaining.size()-1);
            }
        }

        // Split by " and " (original C logic)
        while (!remaining.empty()) {
            int andPos = remaining.find(" and ");
            String authorName;

            if (andPos != -1) {
                authorName = remaining.substr(0, andPos).trim();
                remaining = remaining.substr(andPos + 5).trim();
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
        // Simplified cleanup
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

    // + operator for combining bibliographies (as required)
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
        publications[publicationCount] = pub;
        publicationCount++;
    }

    int getPublicationCount() const { return publicationCount; }

    Publication getPublication(int index) const {
        if (index < 0 || index >= publicationCount) {
            SystemInterface::write(STDERR_FILENO, "Publication index out of bounds\n", 33);
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

    // File I/O following original C parsing logic exactly
    bool loadFromBibFile(const String& filename) {
        int fd = SystemInterface::open(filename.c_str(), O_RDONLY);
        if (fd < 0) {
            SystemInterface::write(STDERR_FILENO, "Error: Cannot open file ", 25);
            filename.print();
            SystemInterface::write(STDERR_FILENO, "\n", 1);
            return false;
        }

        SystemInterface::write(STDOUT_FILENO, "Parsing BibTeX file: ", 21);
        filename.println();

        char buffer[1024];
        String currentLine;
        Publication currentPub;
        bool inEntry = false;
        int totalEntries = 0;

        // Read file character by character to build lines
        ssize_t bytesRead;
        int bufferPos = 0;

        while ((bytesRead = SystemInterface::read(fd, buffer + bufferPos, 1)) > 0) {
            if (buffer[bufferPos] == '\n') {
                buffer[bufferPos] = '\0';
                currentLine = String(buffer);
                String trimmedLine = currentLine.trim();

                // Parse line following original C logic exactly
                if (!trimmedLine.empty()) {
                    const char* lineStr = trimmedLine.c_str();

                    // Check if starting new entry
                    if (lineStr[0] == '@') {
                        if (inEntry && currentPub.isValid()) {
                            addPublication(currentPub);
                            currentPub = Publication();
                        }
                        inEntry = true;
                        totalEntries++;
                    }
                    // Check for end of entry
                    else if (inEntry && lineStr[0] == '}' && trimmedLine.size() == 1) {
                        if (currentPub.isValid()) {
                            addPublication(currentPub);
                        }
                        currentPub = Publication();
                        inEntry = false;
                    }
                    // Parse fields
                    else if (inEntry) {
                        // Title field
                        String titleValue = extractFieldValue(trimmedLine, String("title"));
                        if (!titleValue.empty()) {
                            currentPub.setTitle(titleValue);
                        }

                        // Year field
                        String yearValue = extractFieldValue(trimmedLine, String("year"));
                        if (!yearValue.empty()) {
                            currentPub.setYear(yearValue);
                        }

                        // Journal field
                        String journalValue = extractFieldValue(trimmedLine, String("journal"));
                        if (!journalValue.empty()) {
                            currentPub.setJournal(journalValue);
                        }

                        // Author field (following original C logic)
                        String authorValue = extractFieldValue(trimmedLine, String("author"));
                        if (!authorValue.empty()) {
                            extractAuthors(authorValue, currentPub);
                        }

                        // URL fields as requested
                        String pdfValue = extractFieldValue(trimmedLine, String("pdf"));
                        if (pdfValue.empty()) {
                            pdfValue = extractFieldValue(trimmedLine, String("url"));
                        }
                        if (!pdfValue.empty()) {
                            currentPub.setPdfUrl(pdfValue);
                        }

                        String codeValue = extractFieldValue(trimmedLine, String("code"));
                        if (!codeValue.empty()) {
                            currentPub.setSourceCodeUrl(codeValue);
                        }

                        String slidesValue = extractFieldValue(trimmedLine, String("slides"));
                        if (!slidesValue.empty()) {
                            currentPub.setPresentationUrl(slidesValue);
                        }
                    }
                }

                bufferPos = 0;
            } else {
                bufferPos++;
                if (bufferPos >= 1023) {
                    buffer[1023] = '\0';
                    bufferPos = 0;
                }
            }
        }

        // Handle last entry
        if (inEntry && currentPub.isValid()) {
            addPublication(currentPub);
        }

        SystemInterface::close(fd);
        sort(); // Sort after loading

        // Print summary following original C format
        SystemInterface::write(STDOUT_FILENO, "Loaded ", 7);
        printNumber(publicationCount);
        SystemInterface::write(STDOUT_FILENO, " publications\n", 14);

        return true;
    }

    // Institute counting following original C logic
    int countAuthorsFromInstitute(const String& instituteName) const {
        int totalCount = 0;

        SystemInterface::write(STDOUT_FILENO, "Looking for authors from: ", 26);
        instituteName.println();
        SystemInterface::write(STDOUT_FILENO, "\n", 1);

        for (int i = 0; i < publicationCount; i++) {
            int count = publications[i].countAuthorsFromInstitute(instituteName);
            if (count > 0) {
                // Print found authors (following original C format)
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

    // Helper function to print numbers (no printf)
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
        SystemInterface::write(STDOUT_FILENO, " publications) ===\n\n", 21);

        for (int i = 0; i < publicationCount; i++) {
            printNumber(i + 1);
            SystemInterface::write(STDOUT_FILENO, ". ", 2);
            publications[i].printDetailed();
        }
    }

    void printSummary(const String& instituteName) const {
        SystemInterface::write(STDOUT_FILENO, "\n=== Summary ===\n", 18);
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

#endif
