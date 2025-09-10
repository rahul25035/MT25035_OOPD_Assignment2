// String.h - Custom string implementation using SystemInterface (no std::string)
#ifndef STRING_H
#define STRING_H

#include "SystemInterface.h"

class String {
private:
    char* data;
    size_t length;
    size_t capacity;

    // Allocate memory from sbrk and handle errors.
    static void* my_malloc(size_t size) {
        if (size == 0) size = 1;
        void* p = SystemInterface::sbrk((long)size);
        if (p == (void*)-1) {
            SystemInterface::write(STDERR_FILENO, "Memory allocation failed\n", 25);
            SystemInterface::exit(1);
        }
        return p;
    }

    void ensure_capacity(size_t needed) {
        if (needed <= capacity) return;

        // Compute new capacity (at least double until it fits)
        size_t new_capacity = (capacity == 0) ? 16 : capacity;
        while (new_capacity < needed) {
            new_capacity *= 2;
        }

        // Allocate raw memory
        char* new_data = (char*) my_malloc(new_capacity);
        if (!new_data) {  // unlikely, but check
            SystemInterface::write(STDERR_FILENO, "Memory allocation failed\n", 25);
            SystemInterface::exit(1);
        }

        // Initialize entire block to zero to avoid uninitialized reads
        for (size_t i = 0; i < new_capacity; i++) {
            new_data[i] = '\0';
        }

        // If we already had data, copy it (up to old length+1 for null)
        if (data) {
            for (size_t i = 0; i <= length; i++) {
                new_data[i] = data[i];
            }
        }

        // Switch to the new buffer
        data = new_data;
        capacity = new_capacity;
    }



public:
    // Constructors
    String() : data((char*)0), length(0), capacity(0) {
        ensure_capacity(1);
        data[0] = '\0';
    }

    String(const char* str) : data((char*)0), length(0), capacity(0) {
        if (!str) {
            ensure_capacity(1);
            data[0] = '\0';
            return;
        }

        // compute length manually
        size_t i = 0;
        while (str[i]) i++;
        length = i;

        ensure_capacity(length + 1);
        for (size_t j = 0; j <= length; j++) {
            data[j] = str[j];
        }
    }

    String(const String& other) : data((char*)0), length(0), capacity(0) {
        length = other.length;
        ensure_capacity(length + 1);
        for (size_t i = 0; i <= length; i++) {
            data[i] = other.data[i];
        }
    }

    // Destructor
    ~String() {
    }

    // Assignment operators
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

        size_t i = 0;
        while (str[i]) i++;
        length = i;

        ensure_capacity(length + 1);
        for (size_t j = 0; j <= length; j++) {
            data[j] = str[j];
        }
        return *this;
    }

    // Comparison operators
    bool operator<(const String& other) const {
        if (!data && !other.data) return false;
        if (!data) return true;
        if (!other.data) return false;

        size_t i = 0;
        while (i < length && i < other.length) {
            if (data[i] < other.data[i]) return true;
            if (data[i] > other.data[i]) return false;
            i++;
        }
        if (i == length && i == other.length) return false;
        return (i == length) && (other.length != length);
    }

    bool operator==(const String& other) const {
        if (length != other.length) return false;
        for (size_t i = 0; i < length; i++) {
            if (data[i] != other.data[i]) return false;
        }
        return true;
    }

    bool operator!=(const String& other) const {
        return !(*this == other);
    }

    // Concatenation
    String operator+(const String& other) const {
        String result(*this);
        result += other;
        return result;
    }

    String& operator+=(const String& other) {
        if (other.length == 0) return *this;

        size_t new_length = length + other.length;
        ensure_capacity(new_length + 1);

        for (size_t i = 0; i <= other.length; i++) {
            data[length + i] = other.data[i];
        }
        length = new_length;
        return *this;
    }

    // Access operators (with bounds checking)
    char& operator[](size_t index) {
        if (index >= length) {
            SystemInterface::write(STDERR_FILENO, "String index out of bounds\n", 27);
            SystemInterface::exit(1);
        }
        return data[index];
    }

    const char& operator[](size_t index) const {
        if (index >= length) {
            SystemInterface::write(STDERR_FILENO, "String index out of bounds\n", 27);
            SystemInterface::exit(1);
        }
        return data[index];
    }

    // Utilities
    size_t size() const { return length; }
    bool empty() const { return length == 0; }
    const char* c_str() const { return data ? data : ""; }

    // Trim leading/trailing whitespace
    String trim() const {
        if (!data || length == 0) return String();

        size_t start = 0;
        size_t end = (length == 0) ? 0 : (length - 1);

        // Trim leading whitespace
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

        size_t trimmed_length = end - start + 1;
        // Allocate temporary buffer and construct String from it
        char* temp = (char*) my_malloc(trimmed_length + 1);
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
                    result.data[i] = (char)(result.data[i] + ('a' - 'A'));
                }
            }
        }
        return result;
    }

    // Find substring. Returns index or -1 if not found.
    int find(const String& substr) const {
        return find(substr.c_str());
    }

    int find(const char* substr) const {
        if (!data) return -1;
        if (!substr) return -1;

        // compute substr length
        size_t substr_len = 0;
        while (substr[substr_len]) substr_len++;

        if (substr_len == 0) return 0;
        if (substr_len > length) return -1;

        for (size_t i = 0; i <= length - substr_len; i++) {
            bool match = true;
            for (size_t j = 0; j < substr_len; j++) {
                if (data[i + j] != substr[j]) {
                    match = false;
                    break;
                }
            }
            if (match) return (int)i;
        }
        return -1;
    }

    // Substring (start, len). If len==0 -> till end.
    String substr(size_t start, size_t len = 0) const {
        if (!data || start >= length) return String();

        if (len == 0 || start + len > length) len = length - start;

        char* temp = (char*) my_malloc(len + 1);
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

    // I/O helpers
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

#endif // STRING_H