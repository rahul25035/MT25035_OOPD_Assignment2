// String.h - Custom string implementation, NO standard libraries
#ifndef STRING_H
#define STRING_H

#include "SystemInterface.h"

class String {
private:
    char* data;
    size_t length;
    size_t capacity;

    // Memory allocator using system calls
    static void* my_malloc(size_t size) {
        return SystemInterface::sbrk(size);
    }

    void ensure_capacity(size_t needed) {
        if (needed <= capacity) return;

        size_t new_capacity = (capacity == 0) ? 16 : capacity;
        while (new_capacity < needed) {
            new_capacity *= 2;
        }

        char* new_data = (char*)my_malloc(new_capacity);
        if (!new_data) {
            SystemInterface::write(STDERR_FILENO, "Memory allocation failed\n", 25);
            SystemInterface::exit(1);
        }

        if (data) {
            for (size_t i = 0; i <= length; i++) {
                new_data[i] = data[i];
            }
        }

        data = new_data;
        capacity = new_capacity;
    }

public:
    // Constructors
    String() : data(nullptr), length(0), capacity(0) {
        ensure_capacity(1);
        data[0] = '\0';
    }

    String(const char* str) : data(nullptr), length(0), capacity(0) {
        if (!str) {
            ensure_capacity(1);
            data[0] = '\0';
            return;
        }

        // Calculate length manually (no strlen)
        length = 0;
        while (str[length]) length++;

        ensure_capacity(length + 1);
        for (size_t i = 0; i <= length; i++) {
            data[i] = str[i];
        }
    }

    String(const String& other) : data(nullptr), length(0), capacity(0) {
        length = other.length;
        ensure_capacity(length + 1);
        for (size_t i = 0; i <= length; i++) {
            data[i] = other.data[i];
        }
    }

    // Destructor (simplified - can't free with sbrk)
    ~String() {
        // With sbrk, we can't free individual allocations
    }

    // Assignment operator
    String& operator=(const String& other) {
        if (this != &other) {
            length = other.length;
            ensure_capacity(length + 1);
            for (size_t i = 0; i <= length; i++) {
                data[i] = other.data[i];
            }
        }
        return *this;
    }

    String& operator=(const char* str) {
        if (!str) {
            length = 0;
            ensure_capacity(1);
            data[0] = '\0';
            return *this;
        }

        length = 0;
        while (str[length]) length++;

        ensure_capacity(length + 1);
        for (size_t i = 0; i <= length; i++) {
            data[i] = str[i];
        }
        return *this;
    }

    // Comparison operators for sorting
    bool operator<(const String& other) const {
        if (!data && !other.data) return false;
        if (!data) return true;
        if (!other.data) return false;

        // Manual strcmp implementation
        size_t i = 0;
        while (data[i] && other.data[i]) {
            if (data[i] < other.data[i]) return true;
            if (data[i] > other.data[i]) return false;
            i++;
        }
        return data[i] == 0 && other.data[i] != 0;
    }

    bool operator==(const String& other) const {
        if (!data && !other.data) return true;
        if (!data || !other.data) return false;
        if (length != other.length) return false;

        for (size_t i = 0; i < length; i++) {
            if (data[i] != other.data[i]) return false;
        }
        return true;
    }

    bool operator!=(const String& other) const {
        return !(*this == other);
    }

    // String concatenation
    String operator+(const String& other) const {
        String result(*this);
        result += other;
        return result;
    }

    String& operator+=(const String& other) {
        if (!other.data) return *this;

        size_t new_length = length + other.length;
        ensure_capacity(new_length + 1);

        for (size_t i = 0; i <= other.length; i++) {
            data[length + i] = other.data[i];
        }
        length = new_length;
        return *this;
    }

    // Access operators
    char& operator[](size_t index) {
        if (index >= length) {
            SystemInterface::write(STDERR_FILENO, "String index out of bounds\n", 28);
            SystemInterface::exit(1);
        }
        return data[index];
    }

    const char& operator[](size_t index) const {
        if (index >= length) {
            SystemInterface::write(STDERR_FILENO, "String index out of bounds\n", 28);
            SystemInterface::exit(1);
        }
        return data[index];
    }

    // Utility functions
    size_t size() const { return length; }
    bool empty() const { return length == 0; }
    const char* c_str() const { return data ? data : ""; }

    // String operations (following original C logic)
    String trim() const {
        if (!data || length == 0) return String();

        size_t start = 0, end = length - 1;

        // Trim leading whitespace (manual isspace)
        while (start < length && (data[start] == ' ' || data[start] == '\t' || 
               data[start] == '\n' || data[start] == '\r' || data[start] == '\f' || 
               data[start] == '\v')) {
            start++;
        }

        if (start >= length) return String();

        // Trim trailing whitespace
        while (end > start && (data[end] == ' ' || data[end] == '\t' || 
               data[end] == '\n' || data[end] == '\r' || data[end] == '\f' || 
               data[end] == '\v')) {
            end--;
        }

        // Create trimmed string
        size_t trimmed_length = end - start + 1;
        char* temp = (char*)my_malloc(trimmed_length + 1);
        if (!temp) {
            SystemInterface::write(STDERR_FILENO, "Memory allocation failed\n", 25);
            SystemInterface::exit(1);
        }

        for (size_t i = 0; i < trimmed_length; i++) {
            temp[i] = data[start + i];
        }
        temp[trimmed_length] = '\0';

        String result(temp);
        return result;
    }

    String to_lowercase() const {
        String result(*this);
        if (result.data) {
            for (size_t i = 0; i < result.length; i++) {
                if (result.data[i] >= 'A' && result.data[i] <= 'Z') {
                    result.data[i] += 32; // Manual tolower
                }
            }
        }
        return result;
    }

    int find(const String& substr) const {
        return find(substr.c_str());
    }

    int find(const char* substr) const {
        if (!data || !substr || length == 0) return -1;

        size_t substr_len = 0;
        while (substr[substr_len]) substr_len++;

        if (substr_len == 0) return 0;
        if (substr_len > length) return -1;

        // Manual strstr implementation
        for (size_t i = 0; i <= length - substr_len; i++) {
            bool match = true;
            for (size_t j = 0; j < substr_len; j++) {
                if (data[i + j] != substr[j]) {
                    match = false;
                    break;
                }
            }
            if (match) return i;
        }
        return -1;
    }

    String substr(size_t start, size_t len = 0) const {
        if (!data || start >= length) return String();

        if (len == 0) len = length - start;
        if (start + len > length) len = length - start;

        char* temp = (char*)my_malloc(len + 1);
        if (!temp) {
            SystemInterface::write(STDERR_FILENO, "Memory allocation failed\n", 25);
            SystemInterface::exit(1);
        }

        for (size_t i = 0; i < len; i++) {
            temp[i] = data[start + i];
        }
        temp[len] = '\0';

        String result(temp);
        return result;
    }

    // I/O functions (no printf/scanf)
    void print() const {
        if (data && length > 0) {
            SystemInterface::write(STDOUT_FILENO, data, length);
        }
    }

    void println() const {
        print();
        SystemInterface::write(STDOUT_FILENO, "\n", 1);
    }
};

#endif
